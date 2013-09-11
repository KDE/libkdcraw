/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2006-12-09
 * @brief  a tread-safe libraw C++ program interface
 *
 * @author Copyright (C) 2006-2013 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2006-2013 by Marcel Wiesweg
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

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QStringList>

// LibRaw includes

#include <libraw_version.h>
#include <libraw_config.h>

// Local includes

#include "version.h"
#include "rawfiles.h"

namespace KDcrawIface
{

KDcraw::KDcraw()
    : d(new Private(this))
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

bool KDcraw::loadRawPreview(QImage& image, const QString& path)
{
    // In first, try to extract the embedded JPEG preview. Very fast.
    bool ret = loadEmbeddedPreview(image, path);

    if (ret)
        return true;

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

    libraw_processed_image_t* const thumb = raw.dcraw_make_mem_thumb(&ret);
    if(!thumb)
    {
        kDebug() << "LibRaw: failed to run dcraw_make_mem_thumb: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if(thumb->type == LIBRAW_IMAGE_BITMAP)
        Private::createPPMHeader(imgData, thumb);
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
    Private::createPPMHeader(imgData, halfImg);
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

bool KDcraw::loadFullImage(QImage& image, const QString& path, const RawDecodingSettings& settings)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt(rawFiles());
    QString   ext = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    kDebug() << "Try to load full RAW picture...";

    RawDecodingSettings prm = settings;
    prm.sixteenBitsImage    = false;
    QByteArray imgData;
    int width, height, rgbmax;

    KDcraw decoder;
    bool ret = decoder.decodeRAWImage(path, prm, imgData, width, height, rgbmax);
    if (!ret)
    {
        kDebug() << "Failled to load full RAW picture";
        return false;
    }

    uchar* sptr  = (uchar*)imgData.data();
    uchar tmp8[2];

    // Set RGB color components.
    for (int i = 0 ; i < width * height ; ++i)
    {
        // Swap Red and Blue
        tmp8[0] = sptr[2];
        tmp8[1] = sptr[0];
        sptr[0] = tmp8[0];
        sptr[2] = tmp8[1];

        sptr += 3;
    }

    image = QImage(width, height, QImage::Format_ARGB32);
    uint*  dptr  = (uint*)image.bits();
    sptr         = (uchar*)imgData.data();

    for (int i = 0 ; i < width * height ; ++i)
    {
        *dptr++ = qRgba(sptr[2], sptr[1], sptr[0], 0xFF);
        sptr += 3;
    }

    kDebug() << "Load full RAW picture done";

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

    Private::fillIndentifyInfo(&raw, identify);
    raw.recycle();
    return true;
}

// ----------------------------------------------------------------------------------

bool KDcraw::extractRAWData(const QString& filePath, QByteArray& rawData, DcrawInfoContainer& identify, unsigned int shotSelect)
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

    raw.imgdata.params.output_bps  = 16;
    raw.imgdata.params.shot_select = shotSelect;

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
    d->setProgress(0.4);

    ret = raw.raw2image();
    if (ret != LIBRAW_SUCCESS)
    {
        kDebug() << "LibRaw: failed to run raw2image: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }
    d->setProgress(0.6);

    Private::fillIndentifyInfo(&raw, identify);

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }
    d->setProgress(0.8);

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
                    output++;
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
                output++;
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
    return (d->loadFromLibraw(filePath, imageData, width, height, rgbmax));
}

bool KDcraw::decodeRAWImage(const QString& filePath, const RawDecodingSettings& rawDecodingSettings,
                            QByteArray& imageData, int& width, int& height, int& rgbmax)
{
    m_rawDecodingSettings = rawDecodingSettings;
    return (d->loadFromLibraw(filePath, imageData, width, height, rgbmax));
}

bool KDcraw::checkToCancelWaitingData()
{
    return m_cancel;
}

void KDcraw::setWaitingDataProgress(double)
{
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

bool KDcraw::librawUseRawSpeed()
{
#ifdef LIBRAW_USE_RAWSPEED
    return true;
#else
    return false;
#endif
}

bool KDcraw::librawUseGPL2DemosaicPack()
{
#ifdef LIBRAW_USE_DEMOSAIC_PACK_GPL2
    return true;
#else
    return false;
#endif
}

bool KDcraw::librawUseGPL3DemosaicPack()
{
#ifdef LIBRAW_USE_DEMOSAIC_PACK_GPL3
    return true;
#else
    return false;
#endif
}

}  // namespace KDcrawIface
