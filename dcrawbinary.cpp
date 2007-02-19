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

// KDE includes

#include <kprocess.h>
#include <kmessagebox.h>
#include <kapplication.h>
#include <klocale.h>
#include <kglobal.h>
#include <kaboutdata.h>

// Local includes

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
        version   = QString::null;
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
    KProcess process;
    process.clearArguments();
    process << path();    

    connect(&process, SIGNAL(receivedStderr(KProcess *, char*, int)),
            this, SLOT(slotReadStderrFromDcraw(KProcess*, char*, int)));

    d->available = process.start(KProcess::Block, KProcess::Stderr);
}

void DcrawBinary::slotReadStderrFromDcraw(KProcess*, char* buffer, int buflen)
{
    // The dcraw ouput look like this : Raw Photo Decoder "dcraw" v8.30...
    QString dcrawHeader("Raw Photo Decoder \"dcraw\" v");

    QString stdErr    = QString::fromLocal8Bit(buffer, buflen);
    QString firstLine = stdErr.section('\n', 1, 1);    

    if (firstLine.startsWith(dcrawHeader))
    {
        d->version = firstLine.remove(0, dcrawHeader.length());    
        qDebug("Found dcraw version: %s", version().ascii());    
    }
}

const char *DcrawBinary::path()
{
    return "kdcraw";
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
    // Look into dcraw/dcraw.c implementation.
    return QString("8.41");   
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
    QString appName = KGlobal::instance()->aboutData()->programName();

    if (!isAvailable()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p>Unable to find the <b>%1</b> executable:<br>"
                          "This binary program is required to support Raw file formats. "
                          "You can continue, but you will not be able "
                          "to handle any Raw images. "
                          "Please check the installation of libkdcraw package on your computer.")
                          .arg(path()),
                     QString::null,
                     i18n("Do not show this message again"),
                     KMessageBox::Notify | KMessageBox::AllowLink);
        return;
    }

    if (!versionIsRight()) 
    {
        KMessageBox::information(
                     kapp->activeWindow(),
                     i18n("<qt><p><b>%1</b> executable isn't up to date:<br> "
                          "The version %2 of this binary program have been found on your computer. "
                          "This version is too old to run properly. "
                          "You can continue, but you will not be able "
                          "to handle any Raw images. "
                          "Please check the installation of libkdcraw package on your computer.")
                          .arg(path())
                          .arg(version()),
                     QString::null,
                     i18n("Do not show this message again"),
                     KMessageBox::Notify | KMessageBox::AllowLink);
    }
}

}  // namespace KDcrawIface
