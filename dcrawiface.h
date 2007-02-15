/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at kdemail dot net> 
 *          Marcel Wiesweg <marcel.wiesweg@gmx.de>
 * Date   : 2006-12-09
 * Description : dcraw program interface
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

#ifndef DCRAW_IFACE_H
#define DCRAW_IFACE_H

// Qt Includes.

#include <qstring.h>
#include <qobject.h>
#include <qimage.h>

// Local includes.

#include "libkdcraw_export.h"
#include "rawdecodingsettings.h"
#include "dcrawinfocontainer.h"

class QCustomEvent;

class KProcess;

namespace KDcrawIface
{

class DcrawIfacePriv;

class LIBKDCRAW_EXPORT DcrawIface : public QObject
{
    Q_OBJECT

public:  // Fast non cancelable methods.

    /** Get the embedded preview image in RAW file
    */
    static bool loadDcrawPreview(QImage& image, const QString& path);

    /** Get the camera settings witch have taken RAW file
    */ 
    static bool rawFileIdentify(DcrawInfoContainer& identify, const QString& path);

public:

    DcrawIface();
    ~DcrawIface();

public: // Cancelable methods to extract RAW data. dcraw decoding can take a while.

    /** To cancel 'decodeHalfRAWImage' and 'decodeRAWImage' methods running 
        in a separate thread.
    */
    void cancel();

    /** Extract a small size of decode RAW data in 8 bits/color/pixels 
        using sRGB color space.
    */
    bool decodeHalfRAWImage(const QString& filePath, QString& destPath,
                            RawDecodingSettings rawDecodingSettings);

    /** Extract a full size of RAW data in 8 bits/color/pixels using 
        sRGB color space.
    */
    bool decodeRAWImage(const QString& filePath, QString& destPath,
                        RawDecodingSettings rawDecodingSettings);

private:

    bool loadFromDcraw(const QString& filePath, QString& destPath);
    void startProcess();

    virtual void customEvent(QCustomEvent *);

private slots:

    void slotProcessExited(KProcess *);
    void slotReceivedStdout(KProcess *, char *buffer, int buflen);
    void slotReceivedStderr(KProcess *, char *buffer, int buflen);
    void slotContinueQuery();

private:

    DcrawIfacePriv *d;
};

}  // namespace KDcrawIface

#endif /* DCRAW_IFACE_H */
