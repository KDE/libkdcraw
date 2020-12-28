/*
    SPDX-FileCopyrightText: 2007-2015 Gilles Caulier <caulier dot gilles at gmail dot com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef DCRAW_INFO_CONTAINER_H
#define DCRAW_INFO_CONTAINER_H

// Qt includes

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QSize>
#include <QtCore/QDebug>

// Local includes

#include "libkdcraw_export.h"

namespace KDcrawIface
{

/** A container for RAW image information
 */
class LIBKDCRAW_EXPORT DcrawInfoContainer
{

public:

    /** The RAW image orientation values
     */
    enum ImageOrientation
    {
        ORIENTATION_NONE        = 0,
        ORIENTATION_180         = 3,
        ORIENTATION_Mirror90CCW = 4,
        ORIENTATION_90CCW       = 5,
        ORIENTATION_90CW        = 6
    };

public:

    /** Standard constructor */
    DcrawInfoContainer();

    /** Standard destructor */
    virtual ~DcrawInfoContainer();

    /** Return 'true' if container is empty, else 'false' */
    bool isEmpty();

public:

    /** True if RAW file include an ICC color profile. */
    bool             hasIccProfile;
    /** True is RAW file is decodable by dcraw. */
    bool             isDecodable;

    /** The number of RAW colors. */
    int              rawColors;

    /** The number of RAW images. */
    int              rawImages;

    /** Black level from Raw histogram. */
    unsigned int     blackPoint;

    /** Channel black levels from Raw histogram. */
    unsigned int     blackPointCh[4];

    /** White level from Raw histogram. */
    unsigned int     whitePoint;

    /** Top margin of raw image. */
    unsigned int     topMargin;

    /** Left margin of raw image. */
    unsigned int     leftMargin;

    /** The raw image orientation */
    ImageOrientation orientation;

    /** The sensitivity in ISO used by camera to take the picture. */
    float            sensitivity;

    /** ==> 1/exposureTime = exposure time in seconds. */
    float            exposureTime;
    /** ==> Aperture value in APEX. */
    float            aperture;
    /** ==> Focal Length value in mm. */
    float            focalLength;
    /** The pixel Aspect Ratio if != 1.0. NOTE: if == 1.0, dcraw do not show this value. */
    float            pixelAspectRatio;

    /** White color balance settings. */
    double           daylightMult[3];
    /** Camera multipliers used for White Balance adjustments */
    double           cameraMult[4];

    /** Camera Color Matrix */
    float            cameraColorMatrix1[3][4];
    float            cameraColorMatrix2[3][4];
    float            cameraXYZMatrix[4][3];

    /** The used Color Keys */
    QString          colorKeys;

    /** The camera maker. */
    QString          make;
    /** The camera model. */
    QString          model;
    /** The artist name who have picture owner. */
    QString          owner;
    /** The demosaising filter pattern. */
    QString          filterPattern;
    /** The DNG version. NOTE: it is only shown with DNG RAW files. */
    QString          DNGVersion;

    /** Date & time when the picture has been taken. */
    QDateTime        dateTime;

    /** The image dimensions in pixels. */
    QSize            imageSize;

    /** The thumb dimensions in pixels. */
    QSize            thumbSize;

    /** The full RAW image dimensions in pixels. */
    QSize            fullSize;

    /** The output dimensions in pixels. */
    QSize            outputSize;
};

//! qDebug() stream operator. Writes container @a c to the debug output in a nicely formatted way.
LIBKDCRAW_EXPORT QDebug operator<<(QDebug dbg, const DcrawInfoContainer& c);

} // namespace KDcrawIface

#endif /* DCRAW_INFO_CONTAINER_H */
