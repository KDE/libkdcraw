/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net>
 *          Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Date   : 2006-04-13
 * Description : detection of dcraw binary program
 *
 * Copyright 2006-2007 by Gilles Caulier and Marcel Wiesweg
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

#ifndef DCRAW_BINARY_H
#define DCRAW_BINARY_H

// Qt includes.

#include <qstring.h>
#include <qobject.h>

// Local Includes.

#include "libkdcraw_export.h"

class KProcess;

namespace KDcrawIface
{

class DcrawBinaryPriv;

class LIBKDCRAW_EXPORT DcrawBinary : public QObject
{
    Q_OBJECT

public:

    DcrawBinary();
    ~DcrawBinary();

    static const char *path();
    static QString internalVersion();

    bool           isAvailable() const;
    QString        version() const;
    bool           versionIsRight() const;

    void checkSystem();
    void checkReport();

private slots:

    void slotReadStderrFromDcraw(KProcess*, char*, int);

private:

    DcrawBinaryPriv *d;
};

} // namespace KDcrawIface

#endif  // DCRAW_BINARY_H
