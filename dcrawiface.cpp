/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net> 
 *          Marcel Wiesweg <marcel.wiesweg@gmx.de>
 * Date   : 2006-12-09
 * Description : dcraw program interface
 *
 * Copyright 2006-2007 by Gilles Caulier and Marcel Wiesweg
 *
 * NOTE: Do not use kdDebug() in this library because it will
 *       be used in multithreaded implementations. Use qDebug()
 *       instead. See B.K.O #133026 for details.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
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
#include <sys/ipc.h>
#include <sys/shm.h>
}

// C++ includes.

#include <cstdlib>
#include <cstdio>

// Qt Includes.

#include <qfile.h>
#include <qtimer.h>
#include <qcstring.h>
#include <qfileinfo.h>
#include <qapplication.h>
#include <qmutex.h>
#include <qwaitcondition.h>

// KDE includes.

#include <klocale.h>
#include <kprocess.h>
#include <kstandarddirs.h>

// Local includes.

#include "rawfiles.h"
#include "dcrawbinary.h"
#include "dcrawiface.h"
#include "dcrawiface.moc"

namespace KDcrawIface
{

class DcrawIfacePriv
{
public:

    DcrawIfacePriv()
    {
        cancel     = false;
        running    = false;
        normalExit = false;
        process    = 0;
        queryTimer = 0;
        data       = 0;
        dataPos    = 0;
        width      = 0;
        height     = 0;
        rgbmax     = 0;
    }

    bool                cancel;
    bool                running;
    bool                normalExit;

    uchar              *data;
    
    int                 dataPos;
    int                 width;
    int                 height;
    int                 rgbmax;

    QString             filePath;

    QMutex              mutex;
    
    QWaitCondition      condVar;

    QTimer             *queryTimer;
    
    KProcess           *process;

    RawDecodingSettings rawDecodingSettings;
};

DcrawIface::DcrawIface()
{
    d = new DcrawIfacePriv;
}

DcrawIface::~DcrawIface()
{
    cancel();
    delete d;
}

void DcrawIface::cancel()
{
    d->cancel = true;
}

bool DcrawIface::loadDcrawPreview(QImage& image, const QString& path)
{
    FILE       *f=NULL;
    QByteArray  imgData;
    const int   MAX_IPC_SIZE = (1024*32);
    char        buffer[MAX_IPC_SIZE];
    QFile       file;
    Q_LONG      len;
    QCString    command;

    QFileInfo fileInfo(path);
    QString   rawFilesExt(raw_file_extentions);
    QString ext = fileInfo.extension(false).upper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.upper().contains(ext))
        return false;

    // Try to extract embedded thumbnail using dcraw with options:
    // -c : write to stdout
    // -e : Extract the camera-generated thumbnail, not the raw image (JPEG or a PPM file).
    // Note : this code require at least dcraw version 8.x

    command  = DcrawBinary::path();
    command += " -c -e ";
    command += QFile::encodeName( KProcess::quote( path ) );
    qDebug("Running RAW decoding command: %s", (const char*)command);

    f = popen( command.data(), "r" );

    if ( f == NULL )
        return false;

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            file.close();
            return false;
        }
        else
        {
            int oldSize = imgData.size();
            imgData.resize( imgData.size() + len );
            memcpy(imgData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );

    if ( !imgData.isEmpty() )
    {
        if (image.loadFromData( imgData ))
        {
            qDebug("Using embedded RAW preview extraction");
            return true;
        }
    }

    // In second, try to use simple RAW extraction method in 8 bits ppm output.
    // -c : write to stdout
    // -h : Half-size color image (3x faster than -q)
    // -a : Use automatic white balance
    // -w : Use camera white balance, if possible

    f=NULL;
    command  = DcrawBinary::path();
    command += " -c -h -w -a ";
    command += QFile::encodeName( KProcess::quote( path ) );
    qDebug("Running RAW decoding command: %s", (const char*)command);

    f = popen( command.data(), "r" );

    if ( f == NULL )
        return false;

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            file.close();
            return false;
        }
        else
        {
            int oldSize = imgData.size();
            imgData.resize( imgData.size() + len );
            memcpy(imgData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );

    if ( !imgData.isEmpty() )
    {
        if (image.loadFromData( imgData ))
        {
            qDebug("Using reduced RAW picture extraction");
            return true;
        }
    }

    return false;
}

bool DcrawIface::rawFileIdentify(DcrawInfoContainer& identify, const QString& path)
{
    FILE       *f=NULL;
    QByteArray  txtData;
    const int   MAX_IPC_SIZE = (1024*32);
    char        buffer[MAX_IPC_SIZE];
    QFile       file;
    Q_LONG      len;
    QCString    command;

    QFileInfo fileInfo(path);
    QString   rawFilesExt(raw_file_extentions);
    QString ext = fileInfo.extension(false).upper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.upper().contains(ext))
        return false;

    // Try to get camera maker/model using dcraw with options:
    // -c : write to stdout
    // -i : identify files without decoding them.
    // -v : verbose mode.

    command  = DcrawBinary::path();
    command += " -c -i -v ";
    command += QFile::encodeName( KProcess::quote( path ) );
    qDebug("Running RAW decoding command: %s", (const char*)command);

    f = popen( command.data(), "r" );

    if ( f == NULL )
    {
        identify = DcrawInfoContainer();
        return false;
    }

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
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
    pos = dcrawInfo.find(timeStampHeader);
    if (pos != -1)
    {
        QString timeStamp = dcrawInfo.mid(pos).section('\n', 0, 0);
        timeStamp.remove(0, timeStampHeader.length());
        identify.dateTime = QDateTime::fromString(timeStamp);
    }

    // Extract Camera Maker.
    QString makeHeader("Make: ");
    pos = dcrawInfo.find(makeHeader);
    if (pos != -1)
    {
        QString make = dcrawInfo.mid(pos).section('\n', 0, 0);
        make.remove(0, makeHeader.length());
        identify.make = make;
    }


    // Extract Camera Model.
    QString modelHeader("Model: ");
    pos = dcrawInfo.find(modelHeader);
    if (pos != -1)
    {
        QString model = dcrawInfo.mid(pos).section('\n', 0, 0);
        model.remove(0, modelHeader.length());
        identify.model = model;
    }

    // Extract ISO Speed.
    QString isoSpeedHeader("ISO speed: ");
    pos = dcrawInfo.find(isoSpeedHeader);
    if (pos != -1)
    {
        QString isoSpeed = dcrawInfo.mid(pos).section('\n', 0, 0);
        isoSpeed.remove(0, isoSpeedHeader.length());
        identify.sensitivity = isoSpeed.toLong();
    }

    // Extract Shutter Speed.
    QString shutterSpeedHeader("Shutter: ");
    pos = dcrawInfo.find(shutterSpeedHeader);
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
    pos = dcrawInfo.find(apertureHeader);
    if (pos != -1)
    {
        QString aperture = dcrawInfo.mid(pos).section('\n', 0, 0);
        aperture.remove(0, apertureHeader.length());
        identify.aperture = aperture.toFloat();
    }

    // Extract Focal Length.
    QString focalLengthHeader("Focal Length: ");
    pos = dcrawInfo.find(focalLengthHeader);
    if (pos != -1)
    {
        QString focalLength = dcrawInfo.mid(pos).section('\n', 0, 0);
        focalLength.remove(0, focalLengthHeader.length());
        focalLength.remove(focalLength.length()-3, 3);    // remove " mm" at end of string.
        identify.focalLength = focalLength.toFloat();
    }

    // Extract Image Size.

    QString imageSizeHeader("Image size:  ");
    pos = dcrawInfo.find(imageSizeHeader);
    if (pos != -1)
    {
        QString imageSize = dcrawInfo.mid(pos).section('\n', 0, 0);
        imageSize.remove(0, imageSizeHeader.length());
        int width  = imageSize.section(" x ", 0, 0).toInt();
        int height = imageSize.section(" x ", 1, 1).toInt();
        identify.imageSize = QSize(width, height);
    }

    // Extract "Has an embedded ICC profile" flag.

    QString hasIccProfileHeader("Embedded ICC profile: ");
    pos = dcrawInfo.find(hasIccProfileHeader);
    if (pos != -1)
    {
        QString hasIccProfile = dcrawInfo.mid(pos).section('\n', 0, 0);
        hasIccProfile.remove(0, hasIccProfileHeader.length());
        if (hasIccProfile.contains("yes"))
            identify.hasIccProfile = true;
        else
            identify.hasIccProfile = false;
    }

    // Extract "Is Decodable" flag.

    QString isDecodableHeader("Decodable with dcraw: ");
    pos = dcrawInfo.find(isDecodableHeader);
    if (pos != -1)
    {
        QString isDecodable = dcrawInfo.mid(pos).section('\n', 0, 0);
        isDecodable.remove(0, isDecodableHeader.length());
        if (isDecodable.contains("yes"))
            identify.isDecodable = true;
        else
            identify.isDecodable = false;
    }

    // Extract "Has Secondary Pixel" flag.

    QString hasSecondaryPixelHeader("Secondary pixels: ");
    pos = dcrawInfo.find(hasSecondaryPixelHeader);
    if (pos != -1)
    {
        QString hasSecondaryPixel = dcrawInfo.mid(pos).section('\n', 0, 0);
        hasSecondaryPixel.remove(0, hasSecondaryPixelHeader.length());
        if (hasSecondaryPixel.contains("yes"))
            identify.hasSecondaryPixel = true;
        else
            identify.hasSecondaryPixel = false;
    }

    // Extract Raw Colors.
    QString rawColorsHeader("Raw colors: ");
    pos = dcrawInfo.find(rawColorsHeader);
    if (pos != -1)
    {
        QString rawColors = dcrawInfo.mid(pos).section('\n', 0, 0);
        rawColors.remove(0, rawColorsHeader.length());
        identify.rawColors = rawColors.toInt();
    }

    // Extract Daylight Multipliers.
    QString daylightMultHeader("Daylight multipliers: ");
    pos = dcrawInfo.find(daylightMultHeader);
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
    pos = dcrawInfo.find(cameraMultHeader);
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

bool DcrawIface::decodeHalfRAWImage(const QString& filePath, QString& destPath,
                                    RawDecodingSettings rawDecodingSettings)
{
    d->rawDecodingSettings = rawDecodingSettings;
    d->rawDecodingSettings.halfSizeColorImage = true;
    d->rawDecodingSettings.outputFileFormat   = RawDecodingSettings::PPM;
    return (loadFromDcraw(filePath, destPath));
}

bool DcrawIface::decodeRAWImage(const QString& filePath, QString& destPath,
                                RawDecodingSettings rawDecodingSettings)
{
    d->rawDecodingSettings = rawDecodingSettings;
    return (loadFromDcraw(filePath, destPath));
}

// ----------------------------------------------------------------------------------

bool DcrawIface::loadFromDcraw(const QString& filePath, QString& destPath)
{
    d->filePath   = filePath;
    d->running    = true;
    d->normalExit = false;
    d->cancel     = false;
    d->process    = 0;
    d->data       = 0;
    d->dataPos    = 0;
    d->width      = 0;
    d->height     = 0;
    d->rgbmax     = 0;

    // trigger startProcess and loop to wait dcraw decoding
    QApplication::postEvent(this, new QCustomEvent(QEvent::User));

    while (d->running && !d->cancel)
    {
        // Waiting for dcraw, is running
        QMutexLocker lock(&d->mutex);
        d->condVar.wait(&d->mutex, 10);
    }

    if (!d->normalExit || d->cancel)
    {
        delete [] d->data;
        d->data = 0;
        return false;
    }

/*
    // -- Use a QImage instance to write IPTC preview and Exif thumbnail -------

    QImage img(d->width, d->height, 32);
    uchar* dptr = img.bits();
    uchar* sptr = d->data;

    // Set RGB color components.
    for (int i = 0 ; i < d->width * d->height ; i++)
    {
        dptr[0] = sptr[2];
        dptr[1] = sptr[1];
        dptr[2] = sptr[0];
        dptr[3] = 0xFF;
        dptr += 4;
        sptr += 3;
    }

    QImage iptcPreview   = img.scale(800, 600, QImage::ScaleMin);
    QImage exifThumbnail = iptcPreview.scale(160, 120, QImage::ScaleMin);

    // -- Write image data into destination file -------------------------------

    QByteArray ICCColorProfile = getICCProfilFromFile(d->rawDecodingSettings.outputColorSpace);
    QString soft = QString("Kipi Raw Converter v.%1").arg(kipiplugins_version);
    QFileInfo fi(filePath);
    destPath = fi.dirPath(true) + QString("/") + ".kipi-rawconverter-tmp-" 
                                + QString::number(::time(0));

    switch(d->rawDecodingSettings.outputFileFormat)
    {
        case RawDecodingSettings::JPEG:
        {
            FILE* f = 0;
            f = fopen(QFile::encodeName(destPath), "wb");
    
            if (!f) 
            {
                kdDebug( 51000 ) << "Failed to open JPEG file for writing"
                                 << endl;
                delete [] d->data;
                d->data = 0;
                return false;
            }
    
            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr       jerr;
    
            int      row_stride;
            JSAMPROW row_pointer[1];

            // Init JPEG compressor.    
            cinfo.err = jpeg_std_error(&jerr);
            jpeg_create_compress(&cinfo);
            jpeg_stdio_dest(&cinfo, f);
            cinfo.image_width      = d->width;
            cinfo.image_height     = d->height;
            cinfo.input_components = 3;
            cinfo.in_color_space   = JCS_RGB;
            jpeg_set_defaults(&cinfo);
            // B.K.O #130996: set horizontal and vertical Subsampling factor 
            // to 1 for a best quality of color picture compression. 
            cinfo.comp_info[0].h_samp_factor = 1;
            cinfo.comp_info[0].v_samp_factor = 1; 
            jpeg_set_quality(&cinfo, 100, true);
            jpeg_start_compress(&cinfo, true);

            // Write ICC color profil.
            if (!ICCColorProfile.isEmpty())
                write_icc_profile (&cinfo, (JOCTET *)ICCColorProfile.data(), ICCColorProfile.size());

            // Write image data
            row_stride = cinfo.image_width * 3;
            while (!d->cancel && (cinfo.next_scanline < cinfo.image_height))
            {
                row_pointer[0] = d->data + (cinfo.next_scanline * row_stride);
                jpeg_write_scanlines(&cinfo, row_pointer, 1);
            }
            
            jpeg_finish_compress(&cinfo);
            fclose(f);

            // Metadata restoration and update.
            KExiv2Iface::KExiv2 exiv2Iface;
            exiv2Iface.load(filePath);
            exiv2Iface.setImageProgramId(QString("Kipi Raw Converter"), QString(kipiplugins_version));
            exiv2Iface.setImageDimensions(QSize(d->width, d->height));
            exiv2Iface.setExifThumbnail(exifThumbnail);
            exiv2Iface.setExifTagString("Exif.Image.DocumentName", fi.fileName());
            exiv2Iface.save(destPath);
            break;
        }
        case RawDecodingSettings::PNG:
        {
            FILE* f = 0;
            f = fopen(QFile::encodeName(destPath), "wb");
    
            if (!f) 
            {
                kdDebug( 51000 ) << "Failed to open PNG file for writing"
                                 << endl;
                delete [] d->data;
                d->data = 0;
                return false;
            }
    
            png_color_8 sig_bit;
            png_bytep   row_ptr;
            png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            png_infop info_ptr  = png_create_info_struct(png_ptr);
            png_init_io(png_ptr, f);
            png_set_IHDR(png_ptr, info_ptr, d->width, d->height, 8, 
                        PNG_COLOR_TYPE_RGB,        PNG_INTERLACE_NONE, 
                        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
            sig_bit.red   = 8;
            sig_bit.green = 8;
            sig_bit.blue  = 8;
            sig_bit.alpha = 8;
            png_set_sBIT(png_ptr, info_ptr, &sig_bit);
            png_set_compression_level(png_ptr, 9);

            // Write ICC profil.
            if (!ICCColorProfile.isEmpty())
            {
                png_set_iCCP(png_ptr, info_ptr, "icc", PNG_COMPRESSION_TYPE_BASE, 
                             ICCColorProfile.data(), ICCColorProfile.size());
            }    

            QString libpngver(PNG_HEADER_VERSION_STRING);
            libpngver.replace('\n', ' ');
            soft.append(QString(" (%1)").arg(libpngver));
            png_text text;
            text.key  = "Software";
            text.text = (char *)soft.ascii();
            text.compression = PNG_TEXT_COMPRESSION_zTXt;
            png_set_text(png_ptr, info_ptr, &(text), 1);

            // Metadata restoration and update.
            KExiv2Iface::KExiv2 exiv2Iface;
            exiv2Iface.load(filePath);
            exiv2Iface.setImageProgramId(QString("Kipi Raw Converter"), QString(kipiplugins_version));
            exiv2Iface.setImageDimensions(QSize(d->width, d->height));
            exiv2Iface.setExifThumbnail(exifThumbnail);
            exiv2Iface.setImagePreview(iptcPreview);
            exiv2Iface.setExifTagString("Exif.Image.DocumentName", fi.fileName());

            // Store Exif data.
            QByteArray ba = exiv2Iface.getExif();
            const uchar ExifHeader[] = {0x45, 0x78, 0x69, 0x66, 0x00, 0x00};
            QByteArray profile = QByteArray(ba.size() + sizeof(ExifHeader));
            memcpy(profile.data(), ExifHeader, sizeof(ExifHeader));
            memcpy(profile.data()+sizeof(ExifHeader), ba.data(), ba.size());
            writeRawProfile(png_ptr, info_ptr, "exif", profile.data(), (png_uint_32) profile.size());

            // Store Iptc data.
            QByteArray ba2 = exiv2Iface.getIptc();
            writeRawProfile(png_ptr, info_ptr, "iptc", ba2.data(), (png_uint_32) ba2.size());

            png_write_info(png_ptr, info_ptr);
            png_set_shift(png_ptr, &sig_bit);
            png_set_packing(png_ptr);
            unsigned char* ptr = d->data;
    
            for (int y = 0; !d->cancel && (y < d->height); y++)
            {
                row_ptr = (png_bytep) ptr;
                png_write_rows(png_ptr, &row_ptr, 1);
                ptr += (d->width * 3);
            }
    
            png_write_end(png_ptr, info_ptr);
            png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
            png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
            fclose(f);
            break;
        }
        case RawDecodingSettings::TIFF:
        {
            TIFF          *tif=0;
            unsigned char *data=0;
            int            y;
            int            w;
            
            tif = TIFFOpen(QFile::encodeName(destPath), "wb");
    
            if (!tif) 
            {
                kdDebug( 51000 ) << "Failed to open TIFF file for writing"
                                 << endl;
                delete [] d->data;
                d->data = 0;
                return false;
            }
    
            TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,      d->width);
            TIFFSetField(tif, TIFFTAG_IMAGELENGTH,     d->height);
            TIFFSetField(tif, TIFFTAG_ORIENTATION,     ORIENTATION_TOPLEFT);
            TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
            TIFFSetField(tif, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
            TIFFSetField(tif, TIFFTAG_COMPRESSION,     COMPRESSION_ADOBE_DEFLATE);
            TIFFSetField(tif, TIFFTAG_ZIPQUALITY,      9);
            // NOTE : this tag values aren't defined in libtiff 3.6.1. '2' is PREDICTOR_HORIZONTAL.
            //        Use horizontal differencing for images which are
            //        likely to be continuous tone. The TIFF spec says that this
            //        usually leads to better compression.
            //        See this url for more details:
            //        http://www.awaresystems.be/imaging/tiff/tifftags/predictor.html
            TIFFSetField(tif, TIFFTAG_PREDICTOR,       2); 
            TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_RGB);
            w = TIFFScanlineSize(tif);
            TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,    TIFFDefaultStripSize(tif, 0));


            // Metadata restoration and update.
            KExiv2Iface::KExiv2 exiv2Iface;
            exiv2Iface.load(filePath);
            exiv2Iface.setImageProgramId(QString("Kipi Raw Converter"), QString(kipiplugins_version));
            exiv2Iface.setImageDimensions(QSize(d->width, d->height));
            exiv2Iface.setExifThumbnail(exifThumbnail);
            exiv2Iface.setImagePreview(iptcPreview);
            exiv2Iface.setExifTagString("Exif.Image.DocumentName", fi.fileName());

            // Store Exif data.
            // TODO

            // Store Iptc data.
            QByteArray ba2 = exiv2Iface.getIptc(true);
#if defined(TIFFTAG_PHOTOSHOP)
            TIFFSetField (tif, TIFFTAG_PHOTOSHOP,      (uint32)ba2.size(), (uchar *)ba2.data());
#endif

            QString libtiffver(TIFFLIB_VERSION_STR);
            libtiffver.replace('\n', ' ');
            soft.append(QString(" ( %1 )").arg(libtiffver));
            TIFFSetField(tif, TIFFTAG_SOFTWARE,        (const char*)soft.ascii());

            // Write ICC profil.
            if (!ICCColorProfile.isEmpty())
            {
#if defined(TIFFTAG_ICCPROFILE)    
                TIFFSetField(tif, TIFFTAG_ICCPROFILE, (uint32)ICCColorProfile.size(), 
                             (uchar *)ICCColorProfile.data());
#endif      
            }    

            // Write image data
            for (y = 0; !d->cancel && (y < d->height); y++)
            {
                data = d->data + (y * d->width * 3);
                TIFFWriteScanline(tif, data, y, 0);
            }
    
            TIFFClose(tif);
            break;
        }
        case RawDecodingSettings::PPM:
        {
            FILE* f = fopen(QFile::encodeName(destPath), "wb");
            if (!f) 
            {
                kdDebug( 51000 ) << "Failed to open ppm file for writing"
                                 << endl;
                delete [] d->data;
                d->data = 0;
                return false;
            }
    
            fprintf(f, "P6\n%d %d\n255\n", d->width, d->height);
            fwrite(d->data, 1, d->width*d->height*3, f);
            fclose(f);
            break;
        }
        default:
        {
            kdDebug( 51000 ) << "Invalid output file format"
                             << endl;
            delete [] d->data;
            d->data = 0;
            return false;
        }
    }
*/    

    delete [] d->data;
    d->data = 0;

    if (d->cancel)
    {
        ::remove(QFile::encodeName(destPath));
        return false;
    }

    return true;
}

void DcrawIface::customEvent(QCustomEvent *)
{
    // KProcess (because of QSocketNotifier) is not reentrant.
    // We must only use it from the main thread.
    startProcess();

    // set up timer to call continueQuery at regular intervals
    if (d->running)
    {
        d->queryTimer = new QTimer;
        connect(d->queryTimer, SIGNAL(timeout()),
                this, SLOT(slotContinueQuery()));
        d->queryTimer->start(30);
    }
}

void DcrawIface::slotContinueQuery()
{
    // this is called from the timer

    if (d->cancel)
    {
        d->process->kill();
        d->process->wait();
        d->normalExit = false;
    }
}

void DcrawIface::startProcess()
{
    if (d->cancel)
    {
        d->running    = false;
        d->normalExit = false;
        return;
    }

    // create KProcess and build argument list

    d->process = new KProcess;

    connect(d->process, SIGNAL(processExited(KProcess *)),
            this, SLOT(slotProcessExited(KProcess *)));
             
    connect(d->process, SIGNAL(receivedStdout(KProcess *, char *, int)),
            this, SLOT(slotReceivedStdout(KProcess *, char *, int)));
             
    connect(d->process, SIGNAL(receivedStderr(KProcess *, char *, int)),
            this, SLOT(slotReceivedStderr(KProcess *, char *, int)));

    // run dcraw with options:
    // -c : write to stdout
    //
    // -4 : 16bit ppm output
    //
    // -f : Interpolate RGB as four colors. This blurs the image a little, but it eliminates false 2x2 mesh patterns.
    // -a : Use automatic white balance
    // -w : Use camera white balance, if possible
    // -n : Don't clip colors
    // -s : Use secondary pixels (Fuji Super CCD SR only)
    // -q : Use simple bilinear interpolation for quick results. Warning: this option require arguments 
    //      depending dcraw version (look below)
    // -B : Use bilateral filter to smooth noise while preserving edges.
    // -p : Use the input ICC profiles to define the camera's raw colorspace.
    // -o : Use ICC profiles to define the output colorspace.
    // -h : Output a half-size color image. Twice as fast as -q 0.

    *d->process << DcrawBinary::path();
    *d->process << "-c";

    if (d->rawDecodingSettings.sixteenBitsImage)
        *d->process << "-4";

    if (d->rawDecodingSettings.halfSizeColorImage)
        *d->process << "-h";

    if (d->rawDecodingSettings.cameraColorBalance)
        *d->process << "-w";

    if (d->rawDecodingSettings.automaticColorBalance)
        *d->process << "-a";

    if (d->rawDecodingSettings.RGBInterpolate4Colors)
        *d->process << "-f";

    if (d->rawDecodingSettings.SuperCCDsecondarySensor)
        *d->process << "-s";

    *d->process << "-H";
    *d->process << QString::number(d->rawDecodingSettings.unclipColors);

    *d->process << "-b";
    *d->process << QString::number(d->rawDecodingSettings.brightness);

    *d->process << "-q";
    *d->process << QString::number(d->rawDecodingSettings.RAWQuality);

    if (d->rawDecodingSettings.enableNoiseReduction)
    {
        *d->process << "-B";
        *d->process << QString::number(d->rawDecodingSettings.NRSigmaDomain);
        *d->process << QString::number(d->rawDecodingSettings.NRSigmaRange);
    }

    *d->process << "-o";
    *d->process << QString::number( d->rawDecodingSettings.outputColorSpace );

    *d->process << QFile::encodeName( d->filePath );

    QString args;
    for (uint i = 0 ; i < d->process->args().count(); i++)
        args.append(d->process->args()[i]);

    qDebug("Running RAW decoding command: %s", args.ascii());

    // actually start the process
    if ( !d->process->start(KProcess::NotifyOnExit, 
         KProcess::Communication(KProcess::Stdout | KProcess::Stderr)) )
    {
        qWarning("Failed to start RAW decoding");
        delete d->process;
        d->process    = 0;
        d->running    = false;
        d->normalExit = false;
        return;
    }
}

void DcrawIface::slotProcessExited(KProcess *)
{
    // set variables, clean up, wake up loader thread

    QMutexLocker lock(&d->mutex);
    d->running    = false;
    d->normalExit = d->process->normalExit() && d->process->exitStatus() == 0;
    delete d->process;
    d->process    = 0;
    delete d->queryTimer;
    d->queryTimer = 0;
    d->condVar.wakeAll();
}

void DcrawIface::slotReceivedStdout(KProcess *, char *buffer, int buflen)
{
    if (!d->data)
    {
        // first data packet:
        // Parse PPM header to find out size and allocate buffer

        // PPM header is "P6 <width> <height> <maximum rgb value "
        // where the blanks are newline characters

        QString magic = QString::fromAscii(buffer, 2);
        if (magic != "P6") 
        {
            qWarning("Cannot parse header from RAW decoding: Magic is: %s", magic.ascii());
            d->process->kill();
            return;
        }

        // Find the third newline that marks the header end in a dcraw generated ppm.
        int i       = 0;
        int counter = 0;

        while (i < buflen) 
        {
            if (counter == 3) break;
            if (buffer[i] == '\n') 
            {
                counter++;
            }
            ++i;
        }

        QStringList splitlist = QStringList::split("\n", QString::fromAscii(buffer, i));
        QStringList sizes     = QStringList::split(" ", splitlist[1]);
        if (splitlist.size() < 3 || sizes.size() < 2)
        {
            qWarning("Cannot parse header from RAW decoding: Could not split");
            d->process->kill();
            return;
        }

        d->width  = sizes[0].toInt();
        d->height = sizes[1].toInt();
        d->rgbmax = splitlist[2].toInt();

#ifdef ENABLE_DEBUG_MESSAGES
        qDebug("Parsed PPM header: width %i height %i rgbmax %i", d->width, d->height, d->rgbmax);
#endif

        // cut header from data for memcpy below
        buffer += i;
        buflen -= i;

        // allocate buffer
        d->data    = new uchar[d->width * d->height * (d->rawDecodingSettings.sixteenBitsImage ? 6 : 3)];
        d->dataPos = 0;
    }

    // copy data to buffer
    memcpy(d->data + d->dataPos, buffer, buflen);
    d->dataPos += buflen;
}

void DcrawIface::slotReceivedStderr(KProcess *, char *buffer, int buflen)
{
    QCString message(buffer, buflen);
    qDebug("RAW decoding StdErr: %s", (const char*)message);
}

}  // namespace KDcrawIface
