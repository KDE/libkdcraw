/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-09
 * Description : a tread-safe dcraw program interface
 *
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com> 
 * Copyright (C) 2006-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2007-2008 by Guillaume Castagnino <casta at xwing dot info>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

extern "C"
{
#include <unistd.h>
}

// C++ includes.

#include <cstdlib>
#include <cstdio>
#include <cmath>

// Qt includes.

#include <QDebug>
#include <QEvent>
#include <QFile>
#include <QByteArray>
#include <QTimer>
#include <QFileInfo>
#include <QApplication>
#include <QMutex>
#include <QWaitCondition>

// KDE includes.

#include <kprocess.h>
#include <kstandarddirs.h>
#include <kshell.h>

// LibRaw includes.

#include "libraw/libraw.h"

// Local includes.

#include "version.h"
#include "rawfiles.h"
#include "kdcraw.h"
#include "kdcraw.moc"

namespace KDcrawIface
{

class KDcrawPriv
{
public:

    KDcrawPriv()
    {
        process    = 0;
        queryTimer = 0;
        data       = 0;
        width      = 0;
        height     = 0;
        rgbmax     = 0;
        dataPos    = 0;
    }

    char           *data;

    int             dataPos;
    int             width;
    int             height;
    int             rgbmax;

    QString         filePath;

    QMutex          mutex;

    QWaitCondition  condVar;

    QTimer         *queryTimer;

    KProcess       *process;

public:

    static void createPPMHeader(QByteArray& imgData, const libraw_processed_image_t *img)
    {
        QString header = QString("P6\n%1 %2\n%3\n").arg(img->width).arg(img->height).arg((1 << img->bits)-1);
        imgData.append(header.toAscii());
        imgData.append(QByteArray((const char*)img->data, (int)img->data_size));
        imgData.append("\n");
    }
};

KDcraw::KDcraw()
{
    d = new KDcrawPriv;
    m_cancel  = false;
}

KDcraw::~KDcraw()
{
    cancel();
    delete d;
}

QString KDcraw::version()
{
    return QString(kdcraw_version);
}

void KDcraw::cancel()
{
    m_cancel = true;
}

bool KDcraw::loadDcrawPreview(QImage& image, const QString& path)
{
    // In first, try to extrcat the embedded JPEG preview. Very fast.
    bool ret = loadEmbeddedPreview(image, path);
    if (ret) return true;

    // In second, decode and half size of RAW picture. More slow.
    return (loadHalfPreview(image, path));
}

bool KDcraw::loadEmbeddedPreview(QImage& image, const QString& path)
{
    QByteArray imgData;

    if ( loadEmbeddedPreview(imgData, path) )
    {
        if (image.loadFromData( imgData ))
        {
            qDebug() << "Using embedded RAW preview extraction";
            return true;
        }
    }

    return false;
}

bool KDcraw::loadEmbeddedPreview(QByteArray& imgData, const QString& path)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt(rawFiles());
    QString ext = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    LibRaw raw;
    int ret = raw.open_file(QFile::encodeName(path));
    if (ret != LIBRAW_SUCCESS)
    {
        qDebug("LibRaw open file failed!");
        return false;
    }

    ret = raw.unpack_thumb();
    if (ret != LIBRAW_SUCCESS)
    {
        qDebug("LibRaw unpack failed!");
        return false;
    }

    libraw_processed_image_t *thumb = raw.dcraw_make_mem_thumb(&ret);
    if(!thumb)
    {
        qDebug("LibRaw dcraw make mem thumb failed!");
        return false;
    }

    if(thumb->type == LIBRAW_IMAGE_BITMAP)
        KDcrawPriv::createPPMHeader(imgData, thumb);
    else
        imgData = QByteArray((const char*)thumb->data, (int)thumb->data_size);

    if ( !imgData.isEmpty() )
    {
        qDebug("Failed to load JPEG thumb from LibRaw!");
        return false;
    }

    return false;
}

bool KDcraw::loadHalfPreview(QImage& image, const QString& path)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt(rawFiles());
    QString   ext = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    qDebug("Try to use reduced RAW picture extraction");

    LibRaw raw;
    raw.imgdata.params.use_auto_wb   = 1; // Use automatic white balance.
    raw.imgdata.params.use_camera_wb = 1; // Use camera white balance, if possible.
    raw.imgdata.params.half_size     = 1; // Half-size color image (3x faster than -q).

    int ret = raw.open_file(QFile::encodeName(path));
    if (ret != LIBRAW_SUCCESS)
    {
        qDebug("LibRaw open file failed!");
        return false;
    }

    ret = raw.unpack();
    if (ret != LIBRAW_SUCCESS)
    {
        qDebug("LibRaw unpack failed!");
        return false;
    }

    ret = raw.dcraw_process();
    if (ret != LIBRAW_SUCCESS)
    {
        qDebug("LibRaw dcraw process failed!");
        return false;
    }

    libraw_processed_image_t *halfImg = raw.dcraw_make_mem_image(&ret);
    if(!halfImg)
    {
        qDebug("LibRaw dcraw make mem image failed!");
        return false;
    }

    QByteArray imgData;
    KDcrawPriv::createPPMHeader(imgData, halfImg);

    if (!image.loadFromData(imgData))
    {
        qDebug("Failed to load PPM data from LibRaw!");
        return false;
    }

    qDebug("Using reduced RAW picture extraction");
    return true;
}

bool KDcraw::rawFileIdentify(DcrawInfoContainer& identify, const QString& path)
{

return false;

    FILE       *f=NULL;
    QByteArray  txtData;
    const int   MAX_IPC_SIZE = (1024*32);
    char        buffer[MAX_IPC_SIZE];
    QFile       file;
    qint64      len;
    QByteArray  command;

    QFileInfo fileInfo(path);
    QString   rawFilesExt(rawFiles());
    QString ext = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    // Try to get camera maker/model using dcraw with options:
    // -i : identify files without decoding them.
    // -v : verbose mode.

// FIXME    command  = DcrawBinary::path();
    command += " -i -v ";
    command += QFile::encodeName( KShell::quoteArg( path ) );
    qDebug("Running RAW decoding command: %s", (const char*)command);

    f = popen( command.data(), "r" );

    if ( f == NULL )
    {
        identify = DcrawInfoContainer();
        return false;
    }

    file.open(f, QIODevice::ReadOnly);

    while ((len = file.read(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            identify = DcrawInfoContainer();
            return false;
        }
        else
        {
            int oldSize = txtData.size();
            txtData.resize( txtData.size() + len );
            memcpy(txtData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );
    QString dcrawInfo(txtData);

    if ( dcrawInfo.isEmpty() )
    {
        identify = DcrawInfoContainer();
        return false;
    }

    int pos;

    // Extract Time Stamp.
    QString timeStampHeader("Timestamp: ");
    pos = dcrawInfo.indexOf(timeStampHeader);
    if (pos != -1)
    {
        QString timeStamp = dcrawInfo.mid(pos).section('\n', 0, 0);
        timeStamp.remove(0, timeStampHeader.length());
        identify.dateTime = QDateTime::fromString(timeStamp);
    }

    // Extract Camera Maker.
    QString makeHeader("Camera: ");
    pos = dcrawInfo.indexOf(makeHeader);
    if (pos != -1)
    {
        QString make = dcrawInfo.mid(pos).section('\n', 0, 0);
        make.remove(0, makeHeader.length());
        identify.make = make;
    }

    // Extract Camera Model.
    QString modelHeader("Model: ");
    pos = dcrawInfo.indexOf(modelHeader);
    if (pos != -1)
    {
        QString model = dcrawInfo.mid(pos).section('\n', 0, 0);
        model.remove(0, modelHeader.length());
        identify.model = model;
    }

    // Extract Picture Owner.
    QString ownerHeader("Owner: ");
    pos = dcrawInfo.indexOf(ownerHeader);
    if (pos != -1)
    {
        QString owner = dcrawInfo.mid(pos).section('\n', 0, 0);
        owner.remove(0, ownerHeader.length());
        identify.owner = owner;
    }

    // Extract DNG Version.
    QString DNGVersionHeader("DNG Version: ");
    pos = dcrawInfo.indexOf(DNGVersionHeader);
    if (pos != -1)
    {
        QString DNGVersion = dcrawInfo.mid(pos).section('\n', 0, 0);
        DNGVersion.remove(0, DNGVersionHeader.length());
        identify.DNGVersion = DNGVersion;
    }

    // Extract ISO Speed.
    QString isoSpeedHeader("ISO speed: ");
    pos = dcrawInfo.indexOf(isoSpeedHeader);
    if (pos != -1)
    {
        QString isoSpeed = dcrawInfo.mid(pos).section('\n', 0, 0);
        isoSpeed.remove(0, isoSpeedHeader.length());
        identify.sensitivity = isoSpeed.toLong();
    }

    // Extract Shutter Speed.
    QString shutterSpeedHeader("Shutter: ");
    pos = dcrawInfo.indexOf(shutterSpeedHeader);
    if (pos != -1)
    {
        QString shutterSpeed = dcrawInfo.mid(pos).section('\n', 0, 0);
        shutterSpeed.remove(0, shutterSpeedHeader.length());

        if (shutterSpeed.startsWith("1/"))
            shutterSpeed.remove(0, 2);                   // remove "1/" at start of string.

        shutterSpeed.remove(shutterSpeed.length()-4, 4); // remove " sec" at end of string.
        identify.exposureTime = shutterSpeed.toFloat();
    }

    // Extract Aperture.
    QString apertureHeader("Aperture: f/");
    pos = dcrawInfo.indexOf(apertureHeader);
    if (pos != -1)
    {
        QString aperture = dcrawInfo.mid(pos).section('\n', 0, 0);
        aperture.remove(0, apertureHeader.length());
        identify.aperture = aperture.toFloat();
    }

    // Extract Focal Length.
    QString focalLengthHeader("Focal Length: ");
    pos = dcrawInfo.indexOf(focalLengthHeader);
    if (pos != -1)
    {
        QString focalLength = dcrawInfo.mid(pos).section('\n', 0, 0);
        focalLength.remove(0, focalLengthHeader.length());
        focalLength.remove(focalLength.length()-3, 3);    // remove " mm" at end of string.
        identify.focalLength = focalLength.toFloat();
    }

    // Extract Image Size.

    QString imageSizeHeader("Image size:  ");
    pos = dcrawInfo.indexOf(imageSizeHeader);
    if (pos != -1)
    {
        QString imageSize = dcrawInfo.mid(pos).section('\n', 0, 0);
        imageSize.remove(0, imageSizeHeader.length());
        int width  = imageSize.section(" x ", 0, 0).toInt();
        int height = imageSize.section(" x ", 1, 1).toInt();
        identify.imageSize = QSize(width, height);
    }

    // Extract Full RAW image Size.

    QString fullSizeHeader("Full size:   ");
    pos = dcrawInfo.indexOf(fullSizeHeader);
    if (pos != -1)
    {
        QString fullSize = dcrawInfo.mid(pos).section('\n', 0, 0);
        fullSize.remove(0, fullSizeHeader.length());
        int width  = fullSize.section(" x ", 0, 0).toInt();
        int height = fullSize.section(" x ", 1, 1).toInt();
        identify.fullSize = QSize(width, height);
    }

    // Extract Output image Size.

    QString outputSizeHeader("Output size: ");
    pos = dcrawInfo.indexOf(outputSizeHeader);
    if (pos != -1)
    {
        QString outputSize = dcrawInfo.mid(pos).section('\n', 0, 0);
        outputSize.remove(0, outputSizeHeader.length());
        int width  = outputSize.section(" x ", 0, 0).toInt();
        int height = outputSize.section(" x ", 1, 1).toInt();
        identify.outputSize = QSize(width, height);
    }

    // Extract Thumb Size.

    QString thumbSizeHeader("Thumb size:  ");
    pos = dcrawInfo.indexOf(thumbSizeHeader);
    if (pos != -1)
    {
        QString thumbSize = dcrawInfo.mid(pos).section('\n', 0, 0);
        thumbSize.remove(0, thumbSizeHeader.length());
        int width  = thumbSize.section(" x ", 0, 0).toInt();
        int height = thumbSize.section(" x ", 1, 1).toInt();
        identify.thumbSize = QSize(width, height);
    }

    // Extract "Has an embedded ICC profile" flag.

    QString hasIccProfileHeader("Embedded ICC profile: ");
    pos = dcrawInfo.indexOf(hasIccProfileHeader);
    if (pos != -1)
    {
        QString hasIccProfile = dcrawInfo.mid(pos).section('\n', 0, 0);
        hasIccProfile.remove(0, hasIccProfileHeader.length());
        if (hasIccProfile.contains("yes"))
            identify.hasIccProfile = true;
        else
            identify.hasIccProfile = false;
    }

    // Check if picture is decodable

    identify.isDecodable = true;
    pos = dcrawInfo.indexOf("Cannot decode file");
    if (pos != -1)
        identify.isDecodable = false;

    // Extract "Has Secondary Pixel" flag.

    QString hasSecondaryPixelHeader("Secondary pixels: ");
    pos = dcrawInfo.indexOf(hasSecondaryPixelHeader);
    if (pos != -1)
    {
        QString hasSecondaryPixel = dcrawInfo.mid(pos).section('\n', 0, 0);
        hasSecondaryPixel.remove(0, hasSecondaryPixelHeader.length());
        if (hasSecondaryPixel.contains("yes"))
            identify.hasSecondaryPixel = true;
        else
            identify.hasSecondaryPixel = false;
    }

    // Extract Pixel Aspect Ratio.
    QString aspectRatioHeader("Pixel Aspect Ratio: ");
    pos = dcrawInfo.indexOf(aspectRatioHeader);
    if (pos != -1)
    {
        QString aspectRatio = dcrawInfo.mid(pos).section('\n', 0, 0);
        aspectRatio.remove(0, aspectRatioHeader.length());
        identify.pixelAspectRatio = aspectRatio.toFloat();
    }

    // Extract Raw Colors.
    QString rawColorsHeader("Raw colors: ");
    pos = dcrawInfo.indexOf(rawColorsHeader);
    if (pos != -1)
    {
        QString rawColors = dcrawInfo.mid(pos).section('\n', 0, 0);
        rawColors.remove(0, rawColorsHeader.length());
        identify.rawColors = rawColors.toInt();
    }

    // Extract Raw Images.
    QString rawImagesHeader("Number of raw images: ");
    pos = dcrawInfo.indexOf(rawImagesHeader);
    if (pos != -1)
    {
        QString rawImages = dcrawInfo.mid(pos).section('\n', 0, 0);
        rawImages.remove(0, rawImagesHeader.length());
        identify.rawImages = rawImages.toInt();
    }

    // Extract Filter Pattern.
    QString filterHeader("Filter pattern: ");
    pos = dcrawInfo.indexOf(filterHeader);
    if (pos != -1)
    {
        QString filter = dcrawInfo.mid(pos).section('\n', 0, 0);
        filter.remove(0, filterHeader.length());
        identify.filterPattern = filter;
    }

    // Extract Daylight Multipliers.
    QString daylightMultHeader("Daylight multipliers: ");
    pos = dcrawInfo.indexOf(daylightMultHeader);
    if (pos != -1)
    {
        QString daylightMult = dcrawInfo.mid(pos).section('\n', 0, 0);
        daylightMult.remove(0, daylightMultHeader.length());
        identify.daylightMult[0] = daylightMult.section(" ", 0, 0).toDouble();
        identify.daylightMult[1] = daylightMult.section(" ", 1, 1).toDouble();
        identify.daylightMult[2] = daylightMult.section(" ", 2, 2).toDouble();
    }

    // Extract Camera Multipliers.
    QString cameraMultHeader("Camera multipliers: ");
    pos = dcrawInfo.indexOf(cameraMultHeader);
    if (pos != -1)
    {
        QString cameraMult = dcrawInfo.mid(pos).section('\n', 0, 0);
        cameraMult.remove(0, cameraMultHeader.length());
        identify.cameraMult[0] = cameraMult.section(" ", 0, 0).toDouble();
        identify.cameraMult[1] = cameraMult.section(" ", 1, 1).toDouble();
        identify.cameraMult[2] = cameraMult.section(" ", 2, 2).toDouble();
        identify.cameraMult[3] = cameraMult.section(" ", 3, 3).toDouble();
    }

    return true;
}

// ----------------------------------------------------------------------------------

bool KDcraw::decodeHalfRAWImage(const QString& filePath, const RawDecodingSettings& rawDecodingSettings, 
                                QByteArray &imageData, int &width, int &height, int &rgbmax)
{
    m_rawDecodingSettings                    = rawDecodingSettings;
    m_rawDecodingSettings.halfSizeColorImage = true;
    return (loadFromDcraw(filePath, imageData, width, height, rgbmax));
}

bool KDcraw::decodeRAWImage(const QString& filePath, const RawDecodingSettings& rawDecodingSettings, 
                            QByteArray &imageData, int &width, int &height, int &rgbmax)
{
    m_rawDecodingSettings = rawDecodingSettings;
    return (loadFromDcraw(filePath, imageData, width, height, rgbmax));
}

bool KDcraw::checkToCancelWaitingData()
{
    return m_cancel;
}

bool KDcraw::checkToCancelReceivingData()
{
    return m_cancel;
}

void KDcraw::setWaitingDataProgress(double)
{
}

void KDcraw::setReceivingDataProgress(double)
{
}

bool KDcraw::loadFromDcraw(const QString& filePath, QByteArray &imageData, 
                           int &width, int &height, int &rgbmax)
{
    m_cancel = false;

    LibRaw raw;

    // -j : Do not stretch the image to its correct aspect ratio.
    // -S : set White Point value (saturation).
    // -P : Read the dead pixel list from this file.
    // -p : Use ICC profiles to define the camera's raw colorspace or use embeded profile from raw file.

    if (m_rawDecodingSettings.sixteenBitsImage)
        raw.imgdata.params.output_bps = 16;      // (-4) 16bit ppm output

    if (m_rawDecodingSettings.halfSizeColorImage)
        raw.imgdata.params.half_size = 1;       // (-h) Half-size color image (3x faster than -q).

    if (m_rawDecodingSettings.RGBInterpolate4Colors)
        raw.imgdata.params.four_color_rgb = 1;  // (-f) Interpolate RGB as four colors.

    if (m_rawDecodingSettings.DontStretchPixels)
        raw.imgdata.params.use_fuji_rotate = 1; // (-j) Do not stretch the image to its correct aspect ratio.

    // (-H) Unclip highlight color.
    raw.imgdata.params.highlight = m_rawDecodingSettings.unclipColors;


    if (m_rawDecodingSettings.brightness != 1.0)
    {
        // (-b) Set Brightness value.
        raw.imgdata.params.bright = m_rawDecodingSettings.brightness;
    }

    if (m_rawDecodingSettings.enableBlackPoint)
    {
        // (-k) Set Black Point value.
        raw.imgdata.params.user_black = m_rawDecodingSettings.blackPoint;
    }

    if (m_rawDecodingSettings.enableWhitePoint)
    {
        // (-S) Set White Point value (saturation).
        raw.imgdata.params.user_sat = m_rawDecodingSettings.whitePoint;
    }

    if (m_rawDecodingSettings.medianFilterPasses > 0)
    {
        // (-m) After interpolation, clean up color artifacts by repeatedly applying a 3x3 median filter to the R-G and B-G channels.
        raw.imgdata.params.med_passes = m_rawDecodingSettings.medianFilterPasses;
    }

    if (!m_rawDecodingSettings.deadPixelMap.isEmpty())
    {
/*
        TODO
        args << "-P";
        args << QFile::encodeName(m_rawDecodingSettings.deadPixelMap);
*/
    }

    switch (m_rawDecodingSettings.whiteBalance)
    {
        case RawDecodingSettings::NONE:
        {
            break;
        }
        case RawDecodingSettings::CAMERA:
        {
            // (-w) Use camera white balance, if possible.
            raw.imgdata.params.use_camera_wb = 1;
            break;
        }
        case RawDecodingSettings::AUTO:
        {
            // (-a) Use automatic white balance.
            raw.imgdata.params.use_auto_wb = 1;
            break;
        }
        case RawDecodingSettings::CUSTOM:
        {
            /* Convert between Temperature and RGB.
             */
            double T;
            double RGB[3];
            double xD, yD, X, Y, Z;
            DcrawInfoContainer identify;
            T = m_rawDecodingSettings.customWhiteBalance;

            /* Here starts the code picked and adapted from ufraw (0.12.1)
               to convert Temperature + green multiplier to RGB multipliers
            */
            /* Convert between Temperature and RGB.
             * Base on information from http://www.brucelindbloom.com/
             * The fit for D-illuminant between 4000K and 12000K are from CIE
             * The generalization to 2000K < T < 4000K and the blackbody fits
             * are my own and should be taken with a grain of salt.
             */
            const double XYZ_to_RGB[3][3] = {
                { 3.24071,  -0.969258,  0.0556352 },
                {-1.53726,  1.87599,    -0.203996 },
                {-0.498571, 0.0415557,  1.05707 } };
            // Fit for CIE Daylight illuminant
            if (T <= 4000)
            {
                xD = 0.27475e9/(T*T*T) - 0.98598e6/(T*T) + 1.17444e3/T + 0.145986;
            }
            else if (T <= 7000)
            {
                xD = -4.6070e9/(T*T*T) + 2.9678e6/(T*T) + 0.09911e3/T + 0.244063;
            }
            else
            {
                xD = -2.0064e9/(T*T*T) + 1.9018e6/(T*T) + 0.24748e3/T + 0.237040;
            }

            yD     = -3*xD*xD + 2.87*xD - 0.275;
            X      = xD/yD;
            Y      = 1;
            Z      = (1-xD-yD)/yD;
            RGB[0] = X*XYZ_to_RGB[0][0] + Y*XYZ_to_RGB[1][0] + Z*XYZ_to_RGB[2][0];
            RGB[1] = X*XYZ_to_RGB[0][1] + Y*XYZ_to_RGB[1][1] + Z*XYZ_to_RGB[2][1];
            RGB[2] = X*XYZ_to_RGB[0][2] + Y*XYZ_to_RGB[1][2] + Z*XYZ_to_RGB[2][2];
            /* End of the code picked to ufraw
            */

            RGB[1] = RGB[1] / m_rawDecodingSettings.customWhiteBalanceGreen;

            /* By default, decraw override his default D65 WB
               We need to keep it as a basis : if not, colors with some
               DSLR will have a high dominant of color that will lead to
               a completly wrong WB
            */
            if (rawFileIdentify (identify, d->filePath))
            {
                RGB[0] = identify.daylightMult[0] / RGB[0];
                RGB[1] = identify.daylightMult[1] / RGB[1];
                RGB[2] = identify.daylightMult[2] / RGB[2];
            }
            else
            {
                RGB[0] = 1.0 / RGB[0];
                RGB[1] = 1.0 / RGB[1];
                RGB[2] = 1.0 / RGB[2];
                qDebug("Warning: cannot get daylight multipliers");
            }

            // (-r) set Raw Color Balance Multipliers.
            raw.imgdata.params.user_mul[0] = RGB[0];
            raw.imgdata.params.user_mul[1] = RGB[1];
            raw.imgdata.params.user_mul[2] = RGB[2];
            raw.imgdata.params.user_mul[3] = RGB[1];
            break;
        }
        case RawDecodingSettings::AERA:
        {
            // (-A) Calculate the white balance by averaging a rectangular area from image.
            raw.imgdata.params.greybox[0] = m_rawDecodingSettings.whiteBalanceArea.left();
            raw.imgdata.params.greybox[1] = m_rawDecodingSettings.whiteBalanceArea.top();
            raw.imgdata.params.greybox[2] = m_rawDecodingSettings.whiteBalanceArea.width();
            raw.imgdata.params.greybox[3] = m_rawDecodingSettings.whiteBalanceArea.height();
            break;
        }
    }

    // (-q) Use an interpolation method.
    raw.imgdata.params.user_qual = m_rawDecodingSettings.RAWQuality;

    if (m_rawDecodingSettings.enableNoiseReduction)
    {
        // (-n) Use wavelets to erase noise while preserving real detail.
        raw.imgdata.params.threshold = m_rawDecodingSettings.NRThreshold;
    }

    if (m_rawDecodingSettings.enableCACorrection)
    {
        // (-C) Set Correct chromatic aberration correction.
        raw.imgdata.params.aber[0] = m_rawDecodingSettings.caMultiplier[0];
        raw.imgdata.params.aber[2] = m_rawDecodingSettings.caMultiplier[1];
    }

    switch (m_rawDecodingSettings.inputColorSpace)
    {
        case RawDecodingSettings::EMBEDDED:
        {
            /* TODO
            args << "-p";
            args << "embed";
            */
            break;
        }
        case RawDecodingSettings::CUSTOMINPUTCS:
        {
            /* TODO
            if (!m_rawDecodingSettings.inputProfile.isEmpty())
            {
                args << "-p";
                args << QFile::encodeName(m_rawDecodingSettings.inputProfile);
            }*/
            break;
        }
        default:   // No input profile
            break;
    }

    switch (m_rawDecodingSettings.outputColorSpace)
    {
        case RawDecodingSettings::CUSTOMOUTPUTCS:
        {
            /* TODO
            if (!m_rawDecodingSettings.outputProfile.isEmpty())
            {
                args << "-o";
                args << QFile::encodeName(m_rawDecodingSettings.outputProfile);
            }*/
            break;
        }
        default:
        {
            // (-o) Define the output colorspace.
            raw.imgdata.params.output_color = m_rawDecodingSettings.outputColorSpace;
            break;
        }
    }

    int ret = raw.open_file(QFile::encodeName(filePath));
    if (ret != LIBRAW_SUCCESS)
    {
        qDebug("LibRaw open file failed!");
        return false;
    }

    ret = raw.unpack();
    if (ret != LIBRAW_SUCCESS)
    {
        qDebug("LibRaw unpack failed!");
        return false;
    }

    ret = raw.dcraw_process();
    if (ret != LIBRAW_SUCCESS)
    {
        qDebug("LibRaw dcraw process failed!");
        return false;
    }

    libraw_processed_image_t *img = raw.dcraw_make_mem_image(&ret);
    if(!img)
    {
        qDebug("LibRaw dcraw make mem image failed!");
        return false;
    }

    width     = img->width;
    height    = img->height;
    rgbmax    = (1 << img->bits)-1;
    imageData = QByteArray((const char*)img->data, (int)img->data_size);

    qDebug("Raw data info: width %i height %i rgbmax %i", width, height, rgbmax);

    return true;
}

const char *KDcraw::rawFiles()
{
    return raw_file_extentions;
}

QStringList KDcraw::rawFilesList()
{
    QString string = QString::fromLatin1(rawFiles());
    return string.remove("*.").split(' ');
}

int KDcraw::rawFilesVersion()
{
    return raw_file_extensions_version;
}

QStringList KDcraw::supportedCamera()
{

    QFileInfo fi/*( TODO )*/;
    QFile file(fi.path() + QString("/CAMERALIST"));
    if ( !file.open(QIODevice::ReadOnly) ) 
        return QStringList();

    QByteArray data;
    data.resize(file.size());
    QDataStream stream( &file );
    stream.readRawData(data.data(), data.size());
    file.close();

    QString tmp(data);
    QStringList list = tmp.split('\n');
    return list;
}

}  // namespace KDcrawIface
