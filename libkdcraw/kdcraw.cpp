/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-12-09
 * @brief  a tread-safe libraw C++ program interface
 *
 * @author Copyright (C) 2006-2011 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2011 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2007-2008 by Guillaume Castagnino
 *         <a href="mailto:casta at xwing dot info">casta at xwing dot info</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kdcraw.moc"
#include "kdcraw_p.h"

// Qt includes.

#include <QFile>
#include <QFileInfo>
#include <QStringList>

// LibRaw includes.

#include "libraw/libraw_version.h"

// Local includes.

#include "version.h"
#include "rawfiles.h"

namespace KDcrawIface
{

KDcraw::KDcraw()
      : d(new KDcrawPriv(this))
{
    m_cancel = false;
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
    // In first, try to extract the embedded JPEG preview. Very fast.
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
        kDebug() << "Preview data size:" << imgData.size();

        if (image.loadFromData( imgData ))
        {
            kDebug() << "Using embedded RAW preview extraction";
            return true;
        }
    }

    kDebug() << "Failed to load embedded RAW preview";
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
        kDebug() << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    ret = raw.unpack_thumb();
    if (ret != LIBRAW_SUCCESS)
    {
        raw.recycle();
        kDebug() << "LibRaw: failed to run unpack_thumb: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    libraw_processed_image_t* thumb = raw.dcraw_make_mem_thumb(&ret);
    if(!thumb)
    {
        kDebug() << "LibRaw: failed to run dcraw_make_mem_thumb: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if(thumb->type == LIBRAW_IMAGE_BITMAP)
        KDcrawPriv::createPPMHeader(imgData, thumb);
    else
        imgData = QByteArray((const char*)thumb->data, (int)thumb->data_size);

    // Clear memory allocation. Introduced with LibRaw 0.11.0
    raw.dcraw_clear_mem(thumb);
    raw.recycle();

    if ( imgData.isEmpty() )
    {
        kDebug() << "Failed to load JPEG thumb from LibRaw!";
        return false;
    }

    return true;
}

bool KDcraw::loadHalfPreview(QImage& image, const QString& path)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt(rawFiles());
    QString   ext = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    kDebug() << "Try to use reduced RAW picture extraction";

    LibRaw raw;
    raw.imgdata.params.use_auto_wb   = 1;         // Use automatic white balance.
    raw.imgdata.params.use_camera_wb = 1;         // Use camera white balance, if possible.
    raw.imgdata.params.half_size     = 1;         // Half-size color image (3x faster than -q).

    // NOTE: new magic option introduced by LibRaw 0.7.0 to to make better noise filtration.
    raw.imgdata.params.filtering_mode = LIBRAW_FILTERING_AUTOMATIC;

    int ret = raw.open_file(QFile::encodeName(path));
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    ret = raw.unpack();
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run unpack: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    ret = raw.dcraw_process();
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run dcraw_process: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    libraw_processed_image_t* halfImg = raw.dcraw_make_mem_image(&ret);
    if(!halfImg)
    {
        kDebug() << "LibRaw: failed to run dcraw_make_mem_image: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    QByteArray imgData;
    KDcrawPriv::createPPMHeader(imgData, halfImg);
    // Clear memory allocation. Introduced with LibRaw 0.11.0
    raw.dcraw_clear_mem(halfImg);
    raw.recycle();

    if (!image.loadFromData(imgData))
    {
        kDebug() << "Failed to load PPM data from LibRaw!";
        return false;
    }

    kDebug() << "Using reduced RAW picture extraction";

    return true;
}

bool KDcraw::rawFileIdentify(DcrawInfoContainer& identify, const QString& path)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt(rawFiles());
    QString ext          = fileInfo.suffix().toUpper();
    identify.isDecodable = false;

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    LibRaw raw;

    int ret = raw.open_file(QFile::encodeName(path));
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    ret = raw.adjust_sizes_info_only();
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run adjust_sizes_info_only: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    KDcrawPriv::fillIndentifyInfo(&raw, identify);
    raw.recycle();
    return true;
}

// ----------------------------------------------------------------------------------

bool KDcraw::extractRAWData(const QString& filePath, QByteArray& rawData, DcrawInfoContainer& identify)
{
    return extractRAWData(filePath, rawData, identify, false, 0);
}

// ----------------------------------------------------------------------------------

bool KDcraw::extractRAWData(const QString& filePath, QByteArray& rawData, DcrawInfoContainer& identify,
                            bool addMaskedBorders, unsigned int shotSelect)
{
    QFileInfo fileInfo(filePath);
    QString   rawFilesExt(rawFiles());
    QString ext          = fileInfo.suffix().toUpper();
    identify.isDecodable = false;

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    if (m_cancel)
        return false;

    d->setProgress(0.1);

    LibRaw raw;
    // Set progress call back function.
    raw.set_progress_handler(callbackForLibRaw, d);

    int ret = raw.open_file(QFile::encodeName(filePath));
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }
    d->setProgress(0.3);

    raw.imgdata.params.output_bps    = 16;
    raw.imgdata.params.document_mode = 2;
    raw.imgdata.params.shot_select   = shotSelect;

    ret = raw.unpack();
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run unpack: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if (addMaskedBorders)
    {
        ret = raw.add_masked_borders_to_bitmap();
        if (ret != LIBRAW_SUCCESS)
        {
            kDebug() << "LibRaw: failed to add masked borders: " << libraw_strerror(ret);
            raw.recycle();
            return false;
        }
    }

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }
    d->setProgress(0.5);

    KDcrawPriv::fillIndentifyInfo(&raw, identify);

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }
    d->setProgress(0.7);

    rawData = QByteArray();
    
    if (raw.imgdata.idata.filters == 0)
    {
        rawData.resize((int)(raw.imgdata.sizes.iwidth * raw.imgdata.sizes.iheight  * raw.imgdata.idata.colors * sizeof(unsigned short)));
        
        unsigned short* output = (unsigned short*)rawData.data();

        for (unsigned int row = 0; row < raw.imgdata.sizes.iheight; row++)
        {
            for (unsigned int col = 0; col < raw.imgdata.sizes.iwidth; col++)
            {
                for (int color = 0; color < raw.imgdata.idata.colors; color++)
                {
                    *output = raw.imgdata.image[raw.imgdata.sizes.iwidth*row + col][color];
                    *output++;
                }
            }
        }
    }
    else
    {
        rawData.resize((int)(raw.imgdata.sizes.iwidth * raw.imgdata.sizes.iheight * sizeof(unsigned short)));

        unsigned short* output = (unsigned short*)rawData.data();

        for (uint row = 0; row < raw.imgdata.sizes.iheight; row++)
        {
            for (uint col = 0; col < raw.imgdata.sizes.iwidth; col++)
            {
                *output = raw.imgdata.image[raw.imgdata.sizes.iwidth*row + col][raw.COLOR(row, col)];
                *output++;
            }
        }
    }

    raw.recycle();
    d->setProgress(1.0);

    return true;
}

bool KDcraw::decodeHalfRAWImage(const QString& filePath, const RawDecodingSettings& rawDecodingSettings,
                                QByteArray& imageData, int& width, int& height, int& rgbmax)
{
    m_rawDecodingSettings                    = rawDecodingSettings;
    m_rawDecodingSettings.halfSizeColorImage = true;
    return (loadFromLibraw(filePath, imageData, width, height, rgbmax));
}

bool KDcraw::decodeRAWImage(const QString& filePath, const RawDecodingSettings& rawDecodingSettings,
                            QByteArray& imageData, int& width, int& height, int& rgbmax)
{
    m_rawDecodingSettings = rawDecodingSettings;
    return (loadFromLibraw(filePath, imageData, width, height, rgbmax));
}

bool KDcraw::checkToCancelWaitingData()
{
    return m_cancel;
}

void KDcraw::setWaitingDataProgress(double)
{
}

bool KDcraw::loadFromLibraw(const QString& filePath, QByteArray& imageData,
                            int& width, int& height, int& rgbmax)
{
    m_cancel = false;

    LibRaw raw;
    // Set progress call back function.
    raw.set_progress_handler(callbackForLibRaw, d);

    QByteArray deadpixelPath = QFile::encodeName(m_rawDecodingSettings.deadPixelMap);
    QByteArray cameraProfile = QFile::encodeName(m_rawDecodingSettings.inputProfile);
    QByteArray outputProfile = QFile::encodeName(m_rawDecodingSettings.outputProfile);

    // NOTE: new magic option introduced by LibRaw 0.7.0 to to make better noise filtration.
    raw.imgdata.params.filtering_mode = LIBRAW_FILTERING_AUTOMATIC;

    if (!m_rawDecodingSettings.autoBrightness)
    {
        // Use a fixed white level, ignoring the image histogram.
        raw.imgdata.params.no_auto_bright = 1;
    }

    if (m_rawDecodingSettings.sixteenBitsImage)
    {
        // (-4) 16bit ppm output
        raw.imgdata.params.output_bps = 16;
    }

    if (m_rawDecodingSettings.halfSizeColorImage)
    {
        // (-h) Half-size color image (3x faster than -q).
        raw.imgdata.params.half_size = 1;
    }

    if (m_rawDecodingSettings.RGBInterpolate4Colors)
    {
        // (-f) Interpolate RGB as four colors.
        raw.imgdata.params.four_color_rgb = 1;
    }

    if (m_rawDecodingSettings.DontStretchPixels)
    {
        // (-j) Do not stretch the image to its correct aspect ratio.
        raw.imgdata.params.use_fuji_rotate = 1;
    }

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
        // (-P) Read the dead pixel list from this file.
        raw.imgdata.params.bad_pixels = deadpixelPath.data();
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
               a completely wrong WB
            */
            if (rawFileIdentify(identify, filePath))
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
                kDebug() << "Warning: cannot get daylight multipliers";
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

    switch (m_rawDecodingSettings.NRType)
    {
        case RawDecodingSettings::WAVELETSNR:
        {
            // (-n) Use wavelets to erase noise while preserving real detail.
            raw.imgdata.params.threshold    = m_rawDecodingSettings.NRThreshold;
            break;
        }
        case RawDecodingSettings::FBDDNR:
        {
            // (100 - 1000) => (1 - 10) conversion
            raw.imgdata.params.fbdd_noiserd = lround(m_rawDecodingSettings.NRThreshold / 100.0);
            break;
        }
        case RawDecodingSettings::LINENR:
        {
            // (100 - 1000) => (0.001 - 0.02) conversion.
            raw.imgdata.params.linenoise    = m_rawDecodingSettings.NRThreshold * 2.11E-5 + 0.00111111;
            raw.imgdata.params.cfaline      = true;
            break;
        }

        case RawDecodingSettings::IMPULSENR:
        {
            // (100 - 1000) => (0.005 - 0.05) conversion.
            raw.imgdata.params.lclean       = m_rawDecodingSettings.NRThreshold     * 5E-5;
            raw.imgdata.params.cclean       = m_rawDecodingSettings.NRChroThreshold * 5E-5;
            raw.imgdata.params.cfa_clean    = true;
            break;
        }
        default:   // No Noise Reduction
        {
            raw.imgdata.params.threshold    = 0;
            raw.imgdata.params.fbdd_noiserd = 0;
            raw.imgdata.params.linenoise    = 0;
            raw.imgdata.params.cfaline      = false;
            raw.imgdata.params.lclean       = 0;
            raw.imgdata.params.cclean       = 0;
            raw.imgdata.params.cfa_clean    = false;
            break;
        }
    }

    // Chromatic aberration correction.
    raw.imgdata.params.ca_correc  = m_rawDecodingSettings.enableCACorrection;
    raw.imgdata.params.cared      = m_rawDecodingSettings.caMultiplier[0];
    raw.imgdata.params.cablue     = m_rawDecodingSettings.caMultiplier[1];

    // Exposure Correction before interpolation.
    raw.imgdata.params.exp_correc = m_rawDecodingSettings.expoCorrection;
    raw.imgdata.params.exp_shift  = m_rawDecodingSettings.expoCorrectionShift;
    raw.imgdata.params.exp_preser = m_rawDecodingSettings.expoCorrectionHighlight;

    switch (m_rawDecodingSettings.inputColorSpace)
    {
        case RawDecodingSettings::EMBEDDED:
        {
            // (-p embed) Use input profile from RAW file to define the camera's raw colorspace.
            raw.imgdata.params.camera_profile = (char*)"embed";
            break;
        }
        case RawDecodingSettings::CUSTOMINPUTCS:
        {
            if (!m_rawDecodingSettings.inputProfile.isEmpty())
            {
                // (-p) Use input profile file to define the camera's raw colorspace.
                raw.imgdata.params.camera_profile = cameraProfile.data();
            }
            break;
        }
        default:   // No input profile
            break;
    }

    switch (m_rawDecodingSettings.outputColorSpace)
    {
        case RawDecodingSettings::CUSTOMOUTPUTCS:
        {
            if (!m_rawDecodingSettings.outputProfile.isEmpty())
            {
                // (-o) Use ICC profile file to define the output colorspace.
                raw.imgdata.params.output_profile = outputProfile.data();
            }
            break;
        }
        default:
        {
            // (-o) Define the output colorspace.
            raw.imgdata.params.output_color = m_rawDecodingSettings.outputColorSpace;
            break;
        }
    }

    //-- Extended demosaicing settings ----------------------------------------------------------

    raw.imgdata.params.dcb_iterations  = m_rawDecodingSettings.dcbIterations;
    raw.imgdata.params.dcb_enhance_fl  = m_rawDecodingSettings.dcbEnhanceFl;
    raw.imgdata.params.eeci_refine     = m_rawDecodingSettings.eeciRefine;
    raw.imgdata.params.es_med_passes   = m_rawDecodingSettings.esMedPasses;

    //-------------------------------------------------------------------------------------------

    d->setProgress(0.1);

    kDebug() << filePath;
    kDebug() << m_rawDecodingSettings;

    int ret = raw.open_file(QFile::encodeName(filePath));
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }
    d->setProgress(0.2);

    ret = raw.unpack();
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run unpack: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }
    d->setProgress(0.25);

    if (m_rawDecodingSettings.fixColorsHighlights)
    {
        kDebug() << "Applying LibRaw highlights adjustments";
        // 1.0 is fallback to default value
        raw.imgdata.params.adjust_maximum_thr = 1.0;
    }
    else
    {
        kDebug() << "Disabling LibRaw highlights adjustments";
        // 0.0 disables this feature
        raw.imgdata.params.adjust_maximum_thr = 0.0;
    }

    ret = raw.dcraw_process();
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run dcraw_process: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }
    d->setProgress(0.3);

    libraw_processed_image_t* img = raw.dcraw_make_mem_image(&ret);
    if(!img)
    {
        kDebug() << "LibRaw: failed to run dcraw_make_mem_image: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if (m_cancel)
    {
        // Clear memory allocation. Introduced with LibRaw 0.11.0
        raw.dcraw_clear_mem(img);
        raw.recycle();
        return false;
    }
    d->setProgress(0.35);

    width     = img->width;
    height    = img->height;
    rgbmax    = (1 << img->bits)-1;
    imageData = QByteArray((const char*)img->data, (int)img->data_size);
    // Clear memory allocation. Introduced with LibRaw 0.11.0
    raw.dcraw_clear_mem(img);
    raw.recycle();

    if (m_cancel)
    {
        return false;
    }
    d->setProgress(0.4);

    kDebug() << "LibRaw: data info: width=" << width
             << " height=" << height
             << " rgbmax=" << rgbmax;

    return true;
}

const char* KDcraw::rawFiles()
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
    QStringList camera;
    const char** list = LibRaw::cameraList();
    for (int i = 0; i < LibRaw::cameraCount(); i++)
        camera.append(list[i]);

    return camera;
}

QString KDcraw::librawVersion()
{
    return QString(LIBRAW_VERSION_STR).remove("-Release");
}

bool KDcraw::librawUseGomp()
{
#ifdef LIBRAW_USE_OPENMP
    return true;
#else
    return false;
#endif
}

}  // namespace KDcrawIface
