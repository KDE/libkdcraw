/*
    SPDX-FileCopyrightText: 2006-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
    SPDX-FileCopyrightText: 2006-2013 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
    SPDX-FileCopyrightText: 2007-2008 Guillaume Castagnino <casta at xwing dot info>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kdcraw.h"
#include "kdcraw_p.h"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QStringList>

// LibRaw includes

#include <libraw_version.h>

#ifdef LIBRAW_HAS_CONFIG
#include <libraw_config.h>
#endif

// Local includes

#include "libkdcraw_debug.h"
#include "libkdcraw_version.h"
#include "rawfiles.h"

namespace KDcrawIface
{

KDcraw::KDcraw()
    : d(new KDcrawPrivate(this))
{
    m_cancel = false;
}

KDcraw::~KDcraw()
{
    cancel();
}

QString KDcraw::version()
{
    return QString::fromLatin1(KDCRAW_VERSION_STRING);
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
        qCDebug(LIBKDCRAW_LOG) << "Preview data size:" << imgData.size();

        if (image.loadFromData( imgData ))
        {
            qCDebug(LIBKDCRAW_LOG) << "Using embedded RAW preview extraction";
            return true;
        }
    }

    qCDebug(LIBKDCRAW_LOG) << "Failed to load embedded RAW preview";
    return false;
}

bool KDcraw::loadEmbeddedPreview(QByteArray& imgData, const QString& path)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt = QString::fromUtf8(rawFiles());
    QString   ext         = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    LibRaw raw;

    int ret = raw.open_file((const char*)(QFile::encodeName(path)).constData());

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    return (KDcrawPrivate::loadEmbeddedPreview(imgData, raw));
}

bool KDcraw::loadEmbeddedPreview(QByteArray& imgData, const QBuffer& buffer)
{
    QString rawFilesExt = QString::fromUtf8(rawFiles());
    LibRaw  raw;

    QByteArray inData = buffer.data();
    int ret           = raw.open_buffer((void*) inData.data(), (size_t) inData.size());

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run open_buffer: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    return (KDcrawPrivate::loadEmbeddedPreview(imgData, raw));
}

bool KDcraw::loadHalfPreview(QImage& image, const QString& path)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt = QString::fromUtf8(rawFiles());
    QString   ext = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    qCDebug(LIBKDCRAW_LOG) << "Try to use reduced RAW picture extraction";

    LibRaw raw;
    raw.imgdata.params.use_auto_wb   = 1;         // Use automatic white balance.
    raw.imgdata.params.use_camera_wb = 1;         // Use camera white balance, if possible.
    raw.imgdata.params.half_size     = 1;         // Half-size color image (3x faster than -q).

    int ret = raw.open_file((const char*)(QFile::encodeName(path)).constData());

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }


    if(!KDcrawPrivate::loadHalfPreview(image, raw))
    {
        qCDebug(LIBKDCRAW_LOG) << "Failed to get half preview from LibRaw!";
        return false;
    }

    qCDebug(LIBKDCRAW_LOG) << "Using reduced RAW picture extraction";

    return true;
}

bool KDcraw::loadHalfPreview(QByteArray& imgData, const QString& path)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt = QString::fromUtf8(rawFiles());
    QString   ext = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    qCDebug(LIBKDCRAW_LOG) << "Try to use reduced RAW picture extraction";

    LibRaw raw;
    int ret = raw.open_file((const char*)(QFile::encodeName(path)).constData());

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run dcraw_process: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    QImage image;

    if (!KDcrawPrivate::loadHalfPreview(image, raw))
    {
        qCDebug(LIBKDCRAW_LOG) << "KDcraw: failed to get half preview: " << libraw_strerror(ret);
        return false;
    }

    QBuffer buffer(&imgData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPEG"); 

    return true;
}

bool KDcraw::loadHalfPreview(QByteArray& imgData, const QBuffer& inBuffer)
{
    QString rawFilesExt = QString::fromUtf8(rawFiles());
    LibRaw  raw;

    QByteArray inData = inBuffer.data();
    int ret           = raw.open_buffer((void*) inData.data(), (size_t) inData.size());

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run dcraw_make_mem_image: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    QImage image;

    if (!KDcrawPrivate::loadHalfPreview(image, raw))
    {
        qCDebug(LIBKDCRAW_LOG) << "KDcraw: failed to get half preview: " << libraw_strerror(ret);
        return false;
    }

    QBuffer buffer(&imgData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG"); 

    return true;
}

bool KDcraw::loadFullImage(QImage& image, const QString& path, const RawDecodingSettings& settings)
{
    QFileInfo fileInfo(path);
    QString   rawFilesExt = QString::fromUtf8(rawFiles());
    QString   ext         = fileInfo.suffix().toUpper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    qCDebug(LIBKDCRAW_LOG) << "Try to load full RAW picture...";

    RawDecodingSettings prm = settings;
    prm.sixteenBitsImage    = false;
    QByteArray imgData;
    int width, height, rgbmax;

    KDcraw decoder;
    bool ret = decoder.decodeRAWImage(path, prm, imgData, width, height, rgbmax);

    if (!ret)
    {
        qCDebug(LIBKDCRAW_LOG) << "Failed to load full RAW picture";
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

    image      = QImage(width, height, QImage::Format_ARGB32);
    uint* dptr = reinterpret_cast<uint*>(image.bits());
    sptr       = (uchar*)imgData.data();

    for (int i = 0 ; i < width * height ; ++i)
    {
        *dptr++ = qRgba(sptr[2], sptr[1], sptr[0], 0xFF);
        sptr += 3;
    }

    qCDebug(LIBKDCRAW_LOG) << "Load full RAW picture done";

    return true;
}

bool KDcraw::rawFileIdentify(DcrawInfoContainer& identify, const QString& path)
{
    QFileInfo fileInfo(path);
    QString rawFilesExt  = QString::fromUtf8(rawFiles());
    QString ext          = fileInfo.suffix().toUpper();
    identify.isDecodable = false;

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    LibRaw raw;

    int ret = raw.open_file((const char*)(QFile::encodeName(path)).constData());

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    ret = raw.adjust_sizes_info_only();

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run adjust_sizes_info_only: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    KDcrawPrivate::fillIndentifyInfo(&raw, identify);
    raw.recycle();
    return true;
}

// ----------------------------------------------------------------------------------

bool KDcraw::extractRAWData(const QString& filePath, QByteArray& rawData, DcrawInfoContainer& identify, unsigned int shotSelect)
{
    QFileInfo fileInfo(filePath);
    QString rawFilesExt  = QString::fromUtf8(rawFiles());
    QString ext          = fileInfo.suffix().toUpper();
    identify.isDecodable = false;

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.toUpper().contains(ext))
        return false;

    if (m_cancel)
        return false;

    d->setProgress(0.1);

    LibRaw raw;
    // Set progress call back function.
    raw.set_progress_handler(callbackForLibRaw, d.get());

    int ret = raw.open_file((const char*)(QFile::encodeName(filePath)).constData());

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run open_file: " << libraw_strerror(ret);
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
#if LIBRAW_COMPILE_CHECK_VERSION_NOTLESS(0, 21)
    raw.imgdata.rawparams.shot_select = shotSelect;
#else
    raw.imgdata.params.shot_select = shotSelect;
#endif
    ret                            = raw.unpack();

    if (ret != LIBRAW_SUCCESS)
    {
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run unpack: " << libraw_strerror(ret);
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
        qCDebug(LIBKDCRAW_LOG) << "LibRaw: failed to run raw2image: " << libraw_strerror(ret);
        raw.recycle();
        return false;
    }

    if (m_cancel)
    {
        raw.recycle();
        return false;
    }

    d->setProgress(0.6);

    KDcrawPrivate::fillIndentifyInfo(&raw, identify);

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

        unsigned short* output = reinterpret_cast<unsigned short*>(rawData.data());

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

        unsigned short* output = reinterpret_cast<unsigned short*>(rawData.data());

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
    return string.remove(QLatin1String("*.")).split(QLatin1Char(' '));
}

int KDcraw::rawFilesVersion()
{
    return raw_file_extensions_version;
}

QStringList KDcraw::supportedCamera()
{
    QStringList camera;
    const char** const list = LibRaw::cameraList();

    for (int i = 0; i < LibRaw::cameraCount(); i++)
        camera.append(QString::fromUtf8(list[i]));

    return camera;
}

QString KDcraw::librawVersion()
{
    return QString::fromLatin1(LIBRAW_VERSION_STR).remove(QLatin1String("-Release"));
}

int KDcraw::librawUseGomp()
{
#ifdef LIBRAW_HAS_CONFIG
#   ifdef LIBRAW_USE_OPENMP
    return true;
#   else
    return false;
#   endif
#else
    return -1;
#endif
}

int KDcraw::librawUseRawSpeed()
{
#ifdef LIBRAW_HAS_CONFIG
#   ifdef LIBRAW_USE_RAWSPEED
    return true;
#   else
    return false;
#   endif
#else
    return -1;
#endif
}

int KDcraw::librawUseGPL2DemosaicPack()
{
#ifdef LIBRAW_HAS_CONFIG
#   ifdef LIBRAW_USE_DEMOSAIC_PACK_GPL2
    return true;
#   else
    return false;
#   endif
#else
    return -1;
#endif
}

int KDcraw::librawUseGPL3DemosaicPack()
{
#ifdef LIBRAW_HAS_CONFIG
#   ifdef LIBRAW_USE_DEMOSAIC_PACK_GPL3
    return true;
#   else
    return false;
#   endif
#else
    return -1;
#endif
}

}  // namespace KDcrawIface

#include "moc_kdcraw.cpp"
