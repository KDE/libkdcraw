/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2008-10-09
 * @brief  internal private container for KDcraw
 *
 * @author Copyright (C) 2008-2012 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

#include "kdcraw.h"
#include "kdcraw_p.h"

// Qt includes

#include <QString>

namespace KDcrawIface
{

int callbackForLibRaw(void* data, enum LibRaw_progress p, int iteration, int expected)
{
    if (data)
    {
        KDcraw::KDcrawPriv* const d = static_cast<KDcraw::KDcrawPriv*>(data);

        if (d)
        {
            return d->progressCallback(p, iteration, expected);
        }
    }

    return 0;
}

// --------------------------------------------------------------------------------------------------

KDcraw::KDcrawPriv::KDcrawPriv(KDcraw* const p)
{
    m_progress = 0.0;
    m_parent   = p;
}

KDcraw::KDcrawPriv::~KDcrawPriv()
{
}

void KDcraw::KDcrawPriv::createPPMHeader(QByteArray& imgData, libraw_processed_image_t* const img)
{
    QString header = QString("P%1\n%2 %3\n%4\n").arg(img->colors == 3 ? "6" : "5")
                                                .arg(img->width)
                                                .arg(img->height)
                                                .arg((1 << img->bits)-1);
    imgData.append(header.toAscii());
    imgData.append(QByteArray((const char*)img->data, (int)img->data_size));
}

int KDcraw::KDcrawPriv::progressCallback(enum LibRaw_progress p, int iteration, int expected)
{
    kDebug() << "LibRaw progress: " << libraw_strprogress(p) << " pass "
             << iteration << " of " << expected;

    // post a little change in progress indicator to show raw processor activity.
    setProgress(progressValue()+0.01);

    // Clean processing termination by user...
    if (m_parent->checkToCancelWaitingData())
    {
        kDebug() << "LibRaw process terminaison invoked...";
        m_parent->m_cancel = true;
        m_progress         = 0.0;
        return 1;
    }

    // Return 0 to continue processing...
    return 0;
}

void KDcraw::KDcrawPriv::setProgress(double value)
{
    m_progress = value;
    m_parent->setWaitingDataProgress(m_progress);
}

double KDcraw::KDcrawPriv::progressValue() const
{
    return m_progress;
}

void KDcraw::KDcrawPriv::fillIndentifyInfo(LibRaw* const raw, DcrawInfoContainer& identify)
{
    identify.dateTime.setTime_t(raw->imgdata.other.timestamp);
    identify.make             = QString(raw->imgdata.idata.make);
    identify.model            = QString(raw->imgdata.idata.model);
    identify.owner            = QString(raw->imgdata.other.artist);
    identify.DNGVersion       = QString::number(raw->imgdata.idata.dng_version);
    identify.sensitivity      = raw->imgdata.other.iso_speed;
    identify.exposureTime     = raw->imgdata.other.shutter;
    identify.aperture         = raw->imgdata.other.aperture;
    identify.focalLength      = raw->imgdata.other.focal_len;
    identify.imageSize        = QSize(raw->imgdata.sizes.width, raw->imgdata.sizes.height);
    identify.fullSize         = QSize(raw->imgdata.sizes.raw_width, raw->imgdata.sizes.raw_height);
    identify.outputSize       = QSize(raw->imgdata.sizes.iwidth, raw->imgdata.sizes.iheight);
    identify.thumbSize        = QSize(raw->imgdata.thumbnail.twidth, raw->imgdata.thumbnail.theight);
    identify.topMargin        = raw->imgdata.sizes.top_margin;
    identify.leftMargin       = raw->imgdata.sizes.left_margin;
    identify.hasIccProfile    = raw->imgdata.color.profile ? true : false;
    identify.isDecodable      = true;
    identify.pixelAspectRatio = raw->imgdata.sizes.pixel_aspect;
    identify.rawColors        = raw->imgdata.idata.colors;
    identify.rawImages        = raw->imgdata.idata.raw_count;
    identify.blackPoint       = raw->imgdata.color.black;

    for (int ch = 0; ch < 8; ch++)
    {
        identify.blackPointCh[ch] = raw->imgdata.color.cblack[ch];
    }

    identify.whitePoint       = raw->imgdata.color.maximum;
    identify.orientation      = (DcrawInfoContainer::ImageOrientation)raw->imgdata.sizes.flip;

    memcpy(&identify.cameraColorMatrix1, &raw->imgdata.color.cmatrix, sizeof(raw->imgdata.color.cmatrix));
    memcpy(&identify.cameraColorMatrix2, &raw->imgdata.color.rgb_cam, sizeof(raw->imgdata.color.rgb_cam));
    memcpy(&identify.cameraXYZMatrix,    &raw->imgdata.color.cam_xyz, sizeof(raw->imgdata.color.cam_xyz));

    if (raw->imgdata.idata.filters)
    {
        if (!raw->imgdata.idata.cdesc[3])
            raw->imgdata.idata.cdesc[3] = 'G';

        for (int i=0; i < 16; i++)
        {
            identify.filterPattern.append(raw->imgdata.idata.cdesc[raw->COLOR(i >> 1,i & 1)]);
        }

        identify.colorKeys = raw->imgdata.idata.cdesc;
    }

    for(int c = 0 ; c < raw->imgdata.idata.colors ; c++)
    {
        identify.daylightMult[c] = raw->imgdata.color.pre_mul[c];
    }

    if (raw->imgdata.color.cam_mul[0] > 0)
    {
        for(int c = 0 ; c < 4 ; c++)
        {
            identify.cameraMult[c] = raw->imgdata.color.cam_mul[c];
        }
    }
}

}  // namespace KDcrawIface
