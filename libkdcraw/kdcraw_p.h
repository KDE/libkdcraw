/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-10-09
 * Description : internal private container for KDcraw
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef KDCRAWPRIVATE_H
#define KDCRAWPRIVATE_H

// Qt includes.

#include <QByteArray>

// KDE includes.

#include <kdebug.h>

// LibRaw includes.

#include "libraw/libraw.h"

// Local includes.

#include "dcrawinfocontainer.h"

namespace KDcrawIface
{

class KDcraw;

extern "C"
{
    int callbackForLibRaw(void *data, enum LibRaw_progress p, int iteration, int expected);
}

class KDcrawPriv
{

public:

    KDcrawPriv(KDcraw *p);
    ~KDcrawPriv();

public:

    static void createPPMHeader(QByteArray& imgData, libraw_processed_image_t *img);

    static void fillIndentifyInfo(LibRaw *raw, DcrawInfoContainer& identify);

    int progressCallback(enum LibRaw_progress p, int iteration, int expected);

    void setProgress(double value);
    double progressValue();

private:

    double  m_progress;

    KDcraw *m_parent;
};

}  // namespace KDcrawIface

#endif /* KDCRAWPRIVATE_H */
