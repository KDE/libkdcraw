/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-09
 * Description : internal private container for KDcraw
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <QString>

// Local includes.

#include "kdcraw.h"
#include "kdcrawprivate.h"

namespace KDcrawIface
{

int callbackForLibRaw(void *data, enum LibRaw_progress p, int iteration, int expected)
{
    if (data)
    {
        KDcrawPriv *d = static_cast<KDcrawPriv*>(data);
        if (d) return d->progressCallback(p, iteration, expected);
    }

    return 0;
}

KDcrawPriv::KDcrawPriv(KDcraw *p)
{
    m_progress = 0.0;
    m_parent   = p;
}

KDcrawPriv::~KDcrawPriv()
{
}

void KDcrawPriv::createPPMHeader(QByteArray& imgData, libraw_processed_image_t *img)
{
    QString header = QString("P6\n%1 %2\n%3\n").arg(img->width).arg(img->height).arg((1 << img->bits)-1);
    imgData.append(header.toAscii());
    imgData.append(QByteArray((const char*)img->data, (int)img->data_size));
}

int KDcrawPriv::progressCallback(enum LibRaw_progress p, int iteration, int expected)
{
    kDebug(51002) << "LibRaw progress: " << libraw_strprogress(p) << " pass "
                  << iteration << " of " << expected;

    // post a little change in progress indicator to show raw processor activity.
    setProgress(progressValue()+0.01);

    // Clean processing termination by user...
    if(m_parent->checkToCancelWaitingData())
    {
        qDebug() << "LibRaw process terminaison invoked...";
        m_parent->m_cancel = true;
        m_progress = 0.0;
        return 1;
    }

    // Return 0 to continue processing...
    return 0;
}

void KDcrawPriv::setProgress(double value)
{
    m_progress = value;
    m_parent->setWaitingDataProgress(m_progress);
}

double KDcrawPriv::progressValue()
{
    return m_progress;
}

void KDcrawPriv::fillIndentifyInfo(LibRaw *raw, DcrawInfoContainer& identify)
{
    identify.dateTime         = QDateTime::fromTime_t(raw->imgdata.other.timestamp);
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
    identify.hasIccProfile    = raw->imgdata.color.profile ? true : false;
    identify.isDecodable      = true;
    identify.pixelAspectRatio = raw->imgdata.sizes.pixel_aspect;
    identify.rawColors        = raw->imgdata.idata.colors;
    identify.rawImages        = raw->imgdata.idata.raw_count;
    identify.blackPoint       = raw->imgdata.color.black;
    identify.whitePoint       = raw->imgdata.color.maximum;
    memcpy(&identify.cameraColorMatrix1, &raw->imgdata.color.cmatrix, sizeof(raw->imgdata.color.cmatrix));
    memcpy(&identify.cameraColorMatrix2, &raw->imgdata.color.rgb_cam, sizeof(raw->imgdata.color.rgb_cam));

    if (raw->imgdata.idata.filters) 
    {
        if (!raw->imgdata.idata.cdesc[3]) raw->imgdata.idata.cdesc[3] = 'G';
        for (int i=0; i < 16; i++)
            identify.filterPattern.append(raw->imgdata.idata.cdesc[raw->fc(i >> 1,i & 1)]);
    }

    for(int c = 0 ; c < raw->imgdata.idata.colors ; c++)
        identify.daylightMult[c] = raw->imgdata.color.pre_mul[c];

    if (raw->imgdata.color.cam_mul[0] > 0) 
    {
        for(int c = 0 ; c < 4 ; c++) 
            identify.cameraMult[c] = raw->imgdata.color.cam_mul[c];
    }

    // NOTE: since dcraw.c 8.77, this information has disapear...
    identify.hasSecondaryPixel = false;
}

}  // namespace KDcrawIface
