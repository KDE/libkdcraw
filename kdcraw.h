/* ============================================================
 * Authors: Gilles Caulier <caulier dot gilles at gmail dot com> 
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

#ifndef KDCRAW_H
#define KDCRAW_H

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

class KDcrawPriv;

class LIBKDCRAW_EXPORT KDcraw : public QObject
{
    Q_OBJECT

public:

    KDcraw();
    ~KDcraw();

/** Fast and non cancelable methods witch do not require a class instance to run.*/
public:  

    /** Get the embedded preview image from RAW pictures.
    */
    static bool loadDcrawPreview(QImage& image, const QString& path);

    /** Get the camera settings witch have taken RAW file. Look into dcrawinfocontainer.h 
        for more details.
    */ 
    static bool rawFileIdentify(DcrawInfoContainer& identify, const QString& path);

/** Cancelable methods to extract RAW data witch require a class instance to run. 
    RAW pictures decoding can take a while.*/
public: 

    /** To cancel 'decodeHalfRAWImage' and 'decodeRAWImage' methods running 
        in a separate thread.
    */
    void cancel();

    /** Extract a small size of decode RAW data using 'rawDecodingSettings' settings.
    */
    bool decodeHalfRAWImage(const QString& filePath, RawDecodingSettings rawDecodingSettings, 
                            QByteArray &imageData, int &width, int &height, int &rgbmax);

    /** Extract a full size of RAW data using 'rawDecodingSettings' settings.
    */
    bool decodeRAWImage(const QString& filePath, RawDecodingSettings rawDecodingSettings, 
                        QByteArray &imageData, int &width, int &height, int &rgbmax);

protected:
    
    bool m_cancel;

    int  m_dataPos;


    RawDecodingSettings m_rawDecodingSettings;

protected:

    /** Re-implement this method to control the cancelisation of loop witch wait data 
        from RAW decoding process with your propers envirronement. 
        By default, this method check if m_cancel is true.
    */
    virtual bool checkToCancelWaitingData();

    /** Re-implement this method to control the cancelisation of loop witch recieve data 
        from RAW decoding process with your propers envirronement. 
        By default, this method check if m_cancel is true.
    */
    virtual bool checkToCancelRecievingData();

    /** Re-implement this method to control the pseudo progress value during RAW decoding (when dcraw run with an
        internal loop without feedback) with your propers environnement. By default, this method do nothing.
    */
    virtual void setWaitingDataProgress(double value);

    /** Re-implement this method to control the progress value during RAW decoding (when dcraw return data)
        with your propers environnement. By default, this method do nothing.
    */
    virtual void setRecievingDataProgress(double value);

private:

    bool loadFromDcraw(const QString& filePath, QByteArray &imageData, 
                       int &width, int &height, int &rgbmax);
    void startProcess();

    virtual void customEvent(QCustomEvent *);

private slots:

    void slotProcessExited(KProcess *);
    void slotReceivedStdout(KProcess *, char *, int);
    void slotReceivedStderr(KProcess *, char *, int);
    void slotContinueQuery();

private:

    KDcrawPriv *d;
};

}  // namespace KDcrawIface

#endif /* KDCRAW_H */
