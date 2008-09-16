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
#include <qfile.h>
#include <qfileinfo.h>

// KDE includes.

#include "kdeversion.h"

#if KDE_IS_VERSION(4,0,0)
#include "qdebug.h"
#define PRINT_DEBUG qDebug()
#define ENDL
#else
#include "kdebug.h"
#define PRINT_DEBUG kdDebug()
#define ENDL << endl
#endif

// Local includes.

#include "kdcraw.h"

using namespace KDcrawIface;

int main (int argc, char **argv)
{
    if(argc != 2) 
    {
        PRINT_DEBUG << "raw2png - RAW Camera Image to PNG Converter" ENDL;
        PRINT_DEBUG << "Usage: <rawfile>" ENDL;
        return -1;
    }

    QString            filePath(argv[1]);
    QFileInfo          input(filePath);
    QString            previewFilePath(input.baseName() + QString(".preview.png"));
    QFileInfo          previewOutput(previewFilePath);
    QString            halfFilePath(input.baseName() + QString(".half.png"));
    QFileInfo          halfOutput(halfFilePath);
    QImage             image;
    DcrawInfoContainer identify;

    // -----------------------------------------------------------

    PRINT_DEBUG << "raw2png: Identify RAW image from " << input.fileName() ENDL;

    KDcraw rawProcessor;
    if (!rawProcessor.rawFileIdentify(identify, filePath))
    {
        PRINT_DEBUG << "raw2png: Idendify RAW image failed. Aborted..." ENDL;
        return -1;
    }

    int width  = identify.imageSize.width();
    int height = identify.imageSize.height();

    PRINT_DEBUG << "raw2png: Raw image info:" ENDL;
    PRINT_DEBUG << "--- Date:      " << identify.dateTime.toString(Qt::ISODate) ENDL;
    PRINT_DEBUG << "--- Make:      " << identify.make ENDL;
    PRINT_DEBUG << "--- Model:     " << identify.model ENDL;
    PRINT_DEBUG << "--- Size:      " << width << "x" << height ENDL;
    PRINT_DEBUG << "--- Filter:    " << identify.filterPattern ENDL;
    PRINT_DEBUG << "--- Colors:    " << identify.rawColors ENDL;

    // -----------------------------------------------------------

    PRINT_DEBUG << "raw2png: Loading RAW image preview" ENDL;

    if (!rawProcessor.loadDcrawPreview(image, filePath))
    {
        PRINT_DEBUG << "raw2png: Loading RAW image preview failed. Aborted..." ENDL;
        return -1;
    }

    PRINT_DEBUG << "raw2png: Saving preview image to "
                << previewOutput.fileName() << " size ("
                << image.width() << "x" << image.height()
                << ")" ENDL;
    image.save(previewFilePath, "PNG");

    // -----------------------------------------------------------

    PRINT_DEBUG << "raw2png: Loading half RAW image" ENDL;

    image = QImage();
    if (!rawProcessor.loadHalfPreview(image, filePath))
    {
        PRINT_DEBUG << "raw2png: Loading half RAW image failed. Aborted..." ENDL;
        return -1;
    }

    PRINT_DEBUG << "raw2png: Saving half image to "
                << halfOutput.fileName() << " size ("
                << image.width() << "x" << image.height()
                << ")" ENDL;
    image.save(halfFilePath, "PNG");

    return 0;
}
