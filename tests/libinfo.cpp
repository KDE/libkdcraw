/*
    A command line tool to show libkdcraw info

    SPDX-FileCopyrightText: 2013 Gilles Caulier <caulier dot gilles at gmail dot com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Qt includes

#include <QString>
#include <QDebug>

// Local includes

#include <KDCRAW/KDcraw>

using namespace KDcrawIface;

int main(int /*argc*/, char** /*argv*/)
{
    qDebug() << "Libkdcraw version : " << KDcraw::version(),
    qDebug() << "Libraw version    : " << KDcraw::librawVersion();
    qDebug() << "Use OpenMP        : " << KDcraw::librawUseGomp();
    qDebug() << "Use RawSpeed      : " << KDcraw::librawUseRawSpeed();
    qDebug() << "Use GPL2 Pack     : " << KDcraw::librawUseGPL2DemosaicPack();
    qDebug() << "Use GPL3 Pack     : " << KDcraw::librawUseGPL3DemosaicPack();
    qDebug() << "Raw files list    : " << KDcraw::rawFilesList();
    qDebug() << "Raw files version : " << KDcraw::rawFilesVersion();
    qDebug() << "Supported camera  : " << KDcraw::supportedCamera();

    return 0;
}
