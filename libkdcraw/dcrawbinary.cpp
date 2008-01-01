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

// Qt includes.

#include <QFileInfo>

// KDE includes

#include <k3process.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kaboutdata.h>

// Local includes

#include "version.h"
#include "rawfiles.h"
#include "dcrawinfo.h"
#include "dcrawbinary.h"
#include "dcrawbinary.moc"

namespace KDcrawIface
{

class DcrawBinaryPriv
{
public:

    DcrawBinaryPriv()
    {
        available = false;
        version.clear();
    }

    bool    available;

    QString version;
};

DcrawBinary *DcrawBinary::m_instance = 0;

DcrawBinary::DcrawBinary()
           : QObject()
{
    d = new DcrawBinaryPriv;
}

DcrawBinary::~DcrawBinary()
{
    m_instance = 0;
    delete d;
}

DcrawBinary *DcrawBinary::instance()
{
    if (!m_instance)
        m_instance = new DcrawBinary;
    return m_instance;
}

void DcrawBinary::cleanUp()
{
    delete m_instance;
}

void DcrawBinary::checkSystem()
{
    K3Process process;
    process.clearArguments();
    process << path();

    connect(&process, SIGNAL(receivedStdout(K3Process *, char*, int)),
            this, SLOT(slotReadStdoutFromDcraw(K3Process*, char*, int)));

    d->available = process.start(K3Process::Block, K3Process::Stdout);
}

void DcrawBinary::slotReadStdoutFromDcraw(K3Process*, char* buffer, int buflen)
{
    // The dcraw output looks like this : Raw photo decoder "dcraw" v8.77...
    QString dcrawHeader("Raw photo decoder \"dcraw\" v");

    QString dcrawOut  = QString::fromLocal8Bit(buffer, buflen);
    QString firstLine = dcrawOut.section('\n', 1, 1);

    if (firstLine.startsWith(dcrawHeader))
    {
        d->version = firstLine.remove(0, dcrawHeader.length());
        qDebug("Found dcraw version: %s", version().toAscii().constData());
    }
}

const char *DcrawBinary::path()
{
    // return the kdcraw binary program name + soname of library as suffixe
    return dcraw_program_path;
}

const char *DcrawBinary::rawFiles()
{
    return raw_file_extentions;
}

bool DcrawBinary::isAvailable() const
{
    return d->available;
}

QString DcrawBinary::version() const
{
    return d->version;
}

QString DcrawBinary::internalVersion()
{
    // The version of dcraw include with this library.
    return QString(dcraw_program_version);
}

bool DcrawBinary::versionIsRight() const
{
    if (d->version.isNull() || !isAvailable())
        return false;

    if (d->version.toFloat() >= internalVersion().toFloat())
        return true;

    return false;
}

void DcrawBinary::checkReport()
{
    QString appName = KGlobal::mainComponent().aboutData()->programName();

    if (!isAvailable()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p>Unable to find the <b>%1</b> executable:<br>"
                          "This binary program is required to support Raw file formats. "
                          "You can continue, but you will not be able "
                          "to handle any Raw images. "
                          "Please check the installation of libkdcraw package on your computer.",
                          path()),
                     QString(),
                     i18n("Do not show this message again"),
                     KMessageBox::Notify | KMessageBox::AllowLink);
        return;
    }

    if (!versionIsRight()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p><b>%1</b> executable is not up to date:<br> "
                          "The version %2 of this binary program have been found on your computer. "
                          "This version is too old to run properly. "
                          "You can continue, but you will not be able "
                          "to handle any Raw images. "
                          "Please check the installation of libkdcraw package on your computer.",
                          path(),
                          version()),
                     QString(),
                     i18n("Do not show this message again"),
                     KMessageBox::Notify | KMessageBox::AllowLink);
    }
}

QStringList DcrawBinary::supportedCamera()
{
    QFileInfo fi(path());
    QFile file(fi.path() + QString("/CAMERALIST"));
    if ( !file.open(QIODevice::ReadOnly) ) 
        return QStringList();
    
    QByteArray data;
    data.resize(file.size());
    QDataStream stream( &file );
    stream.readRawData(data.data(), data.size());
    file.close();

    QString tmp(data);
    QStringList list = tmp.split('\n');
    return list;
}

}  // namespace KDcrawIface
