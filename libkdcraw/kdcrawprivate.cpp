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

// Qt includes.

#include <QDebug>
#include <QString>

// Local includes.

#include "kdcraw.h"
#include "kdcrawprivate.h"

namespace KDcrawIface
{

int callbackForLibRaw(void *data, enum LibRaw_progress p, int iteration,int expected)
{
    KDcrawPriv *d = static_cast<KDcrawPriv*>(data);
    if (d)
        return d->progressCallback(p, iteration, expected);

    return 0;
}

KDcrawPriv::KDcrawPriv(const KDcraw *p): m_parent(p)
{
}

KDcrawPriv::~KDcrawPriv()
{
}

void KDcrawPriv::createPPMHeader(QByteArray& imgData, const libraw_processed_image_t *img)
{
    QString header = QString("P6\n%1 %2\n%3\n").arg(img->width).arg(img->height).arg((1 << img->bits)-1);
    imgData.append(header.toAscii());
    imgData.append(QByteArray((const char*)img->data, (int)img->data_size));
    imgData.append("\n");
}

int KDcrawPriv::progressCallback(enum LibRaw_progress p, int iteration, int expected)
{
    qDebug() << "LibRaw progress: " << libraw_strprogress(p) << " pass "
             << iteration << " of " << expected;


/*  else if (iteration == 0) // 1st iteration of each step
        printf("Starting %s (expecting %d iterations)\n", libraw_strprogress(p),expected);
    else if (iteration == expected-1)
        printf("%s finished\n",libraw_strprogress(p));
*/

    // Clean processing termination by user...
    if(m_parent->m_cancel) return 1;

    // Return 0 to continue processing...
    return 0;
}

}  // namespace KDcrawIface
