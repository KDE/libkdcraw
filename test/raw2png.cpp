/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2008-15-09
 * @brief  a command line tool to convert RAW file to PNG
 *
 * @author Copyright (C) 2008-2011 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

// Local includes

#include "kdcraw.h"

using namespace KDcrawIface;

int main (int argc, char** argv)
{
    if(argc != 2)
    {
        qDebug() << "raw2png - RAW Camera Image to PNG Converter";
        qDebug() << "Usage: <rawfile>";
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

    qDebug() << "raw2png: Identify RAW image from " << input.fileName();

    KDcraw rawProcessor;
    if (!rawProcessor.rawFileIdentify(identify, filePath))
    {
        qDebug() << "raw2png: Idendify RAW image failed. Aborted...";
        return -1;
    }

    int width  = identify.imageSize.width();
    int height = identify.imageSize.height();

    qDebug() << "raw2png: Raw image info:";
    qDebug() << "--- Date:      " << identify.dateTime.toString(Qt::ISODate);
    qDebug() << "--- Make:      " << identify.make;
    qDebug() << "--- Model:     " << identify.model;
    qDebug() << "--- Size:      " << width << "x" << height;
    qDebug() << "--- Filter:    " << identify.filterPattern;
    qDebug() << "--- Colors:    " << identify.rawColors;

    // -----------------------------------------------------------

    qDebug() << "raw2png: Loading RAW image preview";

    if (!rawProcessor.loadDcrawPreview(image, filePath))
    {
        qDebug() << "raw2png: Loading RAW image preview failed. Aborted...";
        return -1;
    }

    qDebug() << "raw2png: Saving preview image to "
             << previewOutput.fileName() << " size ("
             << image.width() << "x" << image.height()
             << ")";
    image.save(previewFilePath, "PNG");

    // -----------------------------------------------------------

    qDebug() << "raw2png: Loading half RAW image";

    image = QImage();
    if (!rawProcessor.loadHalfPreview(image, filePath))
    {
        qDebug() << "raw2png: Loading half RAW image failed. Aborted...";
        return -1;
    }

    qDebug() << "raw2png: Saving half image to "
             << halfOutput.fileName() << " size ("
             << image.width() << "x" << image.height()
             << ")";
    image.save(halfFilePath, "PNG");

    return 0;
}
