/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-15-09
 * Description : a command line tool to convert RAW file to PNG
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

#include <qstring.h>
#include <qcstring.h>
#include <qfile.h>
#include <qfileinfo.h>

// KDE includes.

#include "kdebug.h"

// Local includes.

#include "kdcraw.h"

using namespace KDcrawIface;

int main (int argc, char **argv)
{
    if(argc != 2) 
    {
        kdDebug() << "raw2png - RAW Camera Image to PNG Converter" << endl;
        kdDebug() << "Usage: <rawfile>" << endl;
        return -1;
    }

    QString            filePath(argv[1]);
    QFileInfo          input(filePath);
    QString            previewFilePath(input.baseName() + QString(".preview.png"));
    QString            pngFilePath(input.baseName() + QString(".png"));
    QImage             targetImg;
    DcrawInfoContainer identify;

    kdDebug() << "raw2png: Identify RAW image from " << input.fileName() << endl;

    KDcraw rawProcessor;
    if (!rawProcessor.rawFileIdentify(identify, filePath))
    {
        kdDebug() << "raw2png: Idendify RAW image failed. Aborted..." << endl;
        return -1;
    }

    int width      = identify.imageSize.width();
    int height     = identify.imageSize.height();

    kdDebug() << "raw2png: Raw image info:" << endl;
    kdDebug() << "--- Date:      " << identify.dateTime.toString(Qt::ISODate) << endl;
    kdDebug() << "--- Make:      " << identify.make << endl;
    kdDebug() << "--- Model:     " << identify.model << endl;
    kdDebug() << "--- Size:      " << width << "x" << height << endl;
    kdDebug() << "--- Filter:    " << identify.filterPattern << endl;
    kdDebug() << "--- Colors:    " << identify.rawColors << endl;

    kdDebug() << "raw2png: Loading RAW image preview" << endl;

    if (!rawProcessor.loadDcrawPreview(targetImg, filePath))
    {
        kdDebug() << "raw2png: Loading RAW image preview failed. Aborted..." << endl;
        return -1;
    }

    kdDebug() << "raw2png: Saving preview image" << endl;
    targetImg.save(previewFilePath, "PNG");

    kdDebug() << "raw2png: PNG conversion complete..." << endl;

    return 0;
}
