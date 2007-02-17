/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at gmail dot com>
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

    static DcrawBinary *instance();
    static void cleanUp();

    /** Return the RAW decoding program name. */ 
    static const char *path();

    /** Return the RAW decoding program version included in this library. */ 
    static QString internalVersion();

    /** Return 'true' if RAW decoding program have be found in your system. */ 
    bool isAvailable() const;

    /** Return the RAW decoding program version found in your system. */ 
    QString version() const;

    /** Return 'true' if RAW decoding program version found in your system is 
        the same than the version provided by this library. */ 
    bool versionIsRight() const;

    /** Check your system to see if RAW decoding program is available. */ 
    void checkSystem();

    /** Report any errors on a message box if RAW decoding program detection 
        on your system failed. */ 
    void checkReport();

private slots:

    void slotReadStderrFromDcraw(KProcess*, char*, int);

private:

    DcrawBinary();
    ~DcrawBinary();

private:

    static DcrawBinary *m_instance;

    DcrawBinaryPriv    *d;
};

} // namespace KDcrawIface

#endif  // DCRAW_BINARY_H
