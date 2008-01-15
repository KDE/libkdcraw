/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-04-13
 * Description : detection of dcraw binary program
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DCRAW_BINARY_H
#define DCRAW_BINARY_H

// Qt includes.

#include <QtCore/QString>

// Local Includes.

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class DcrawBinaryPriv;

class LIBKDCRAW_EXPORT DcrawBinary
{

public:

    static DcrawBinary *instance();
    static void cleanUp();

    /** Return the RAW decoding program name. */ 
    static const char *path();

    /** Return the string of all RAW file type mime supported. */
    static const char *rawFiles();

    /** Return the list of all RAW file type mime supported,
        as a QStringList, without wildcard and suffix dot.  */
    static QStringList rawFilesList();

    /** Returns a version number for the list of supported RAW file types.
        This version is incremented if the list of supported formats has changed
        between library releases. */
    static int rawFilesVersion();

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

    /** Provide a list of supported RAW Camera name. */ 
    QStringList supportedCamera();

private:

    DcrawBinary();
    ~DcrawBinary();

private:

    static DcrawBinary *m_instance;

    DcrawBinaryPriv    *d;
};

} // namespace KDcrawIface

#endif  // DCRAW_BINARY_H
