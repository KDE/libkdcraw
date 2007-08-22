/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-12-09
 * Description : a tread-safe dcraw program interface
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com> 
 * Copyright (C) 2006-2007 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * NOTE: Do not use kdDebug() in this implementation because 
 *       it will be multithreaded. Use qDebug() instead. 
 *       See B.K.O #133026 for details.
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

extern "C"
{
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
}

// C++ includes.

#include <cstdlib>
#include <cstdio>
#include <cmath>

// Qt Includes.

#include <qfile.h>
#include <qtimer.h>
#include <qcstring.h>
#include <qfileinfo.h>
#include <qapplication.h>
#include <qmutex.h>
#include <qwaitcondition.h>

// KDE includes.

#include <kprocess.h>
#include <kstandarddirs.h>

// Local includes.

#include "rawfiles.h"
#include "dcrawbinary.h"
#include "kdcraw.h"
#include "kdcraw.moc"

namespace KDcrawIface
{

class KDcrawPriv
{
public:

    KDcrawPriv()
    {
        running    = false;
        normalExit = false;
        process    = 0;
        queryTimer = 0;
        data       = 0;
        width      = 0;
        height     = 0;
        rgbmax     = 0;
        dataPos    = 0;
    }

    bool                 running;
    bool                 normalExit;

    uchar               *data;

    int                  dataPos;
    int                  width;
    int                  height;
    int                  rgbmax;

    QString              filePath;

    QMutex               mutex;

    QWaitCondition       condVar;

    QTimer              *queryTimer;

    KProcess            *process;
};

KDcraw::KDcraw()
{
    d = new KDcrawPriv;
    m_cancel  = false;
}

KDcraw::~KDcraw()
{
    cancel();
    delete d;
}

void KDcraw::cancel()
{
    m_cancel = true;
}

bool KDcraw::loadDcrawPreview(QImage& image, const QString& path)
{
    // In first, try to extrcat the embedded JPEG preview. Very fast.
    bool ret = loadEmbeddedPreview(image, path);
    if (ret) return true;

    // In second, decode and half size of RAW picture. More slow.
    return (loadHalfPreview(image, path));
}

bool KDcraw::loadEmbeddedPreview(QImage& image, const QString& path)
{
    FILE       *f=NULL;
    QByteArray  imgData;
    const int   MAX_IPC_SIZE = (1024*32);
    char        buffer[MAX_IPC_SIZE];
    QFile       file;
    Q_LONG      len;
    QCString    command;

    QFileInfo fileInfo(path);
    QString   rawFilesExt(raw_file_extentions);
    QString ext = fileInfo.extension(false).upper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.upper().contains(ext))
        return false;

    // Try to extract embedded thumbnail using dcraw with options:
    // -c : write to stdout
    // -e : Extract the camera-generated thumbnail, not the raw image (JPEG or a PPM file).
    // Note : this code require at least dcraw version 8.x

    command  = DcrawBinary::path();
    command += " -c -e ";
    command += QFile::encodeName( KProcess::quote( path ) );
    qDebug("Running RAW decoding command: %s", (const char*)command);

    f = popen( command.data(), "r" );

    if ( f == NULL )
        return false;

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            file.close();
            return false;
        }
        else
        {
            int oldSize = imgData.size();
            imgData.resize( imgData.size() + len );
            memcpy(imgData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );

    if ( !imgData.isEmpty() )
    {
        if (image.loadFromData( imgData ))
        {
            qDebug("Using embedded RAW preview extraction");
            return true;
        }
    }

    return false;
}

bool KDcraw::loadHalfPreview(QImage& image, const QString& path)
{
    FILE       *f=NULL;
    QByteArray  imgData;
    const int   MAX_IPC_SIZE = (1024*32);
    char        buffer[MAX_IPC_SIZE];
    QFile       file;
    Q_LONG      len;
    QCString    command;

    QFileInfo fileInfo(path);
    QString   rawFilesExt(raw_file_extentions);
    QString ext = fileInfo.extension(false).upper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.upper().contains(ext))
        return false;

    // Try to use simple RAW extraction method in 8 bits ppm output.
    // -c : write to stdout
    // -h : Half-size color image (3x faster than -q)
    // -a : Use automatic white balance
    // -w : Use camera white balance, if possible

    f=NULL;
    command  = DcrawBinary::path();
    command += " -c -h -w -a ";
    command += QFile::encodeName( KProcess::quote( path ) );
    qDebug("Running RAW decoding command: %s", (const char*)command);

    f = popen( command.data(), "r" );

    if ( f == NULL )
        return false;

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            file.close();
            return false;
        }
        else
        {
            int oldSize = imgData.size();
            imgData.resize( imgData.size() + len );
            memcpy(imgData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );

    if ( !imgData.isEmpty() )
    {
        if (image.loadFromData( imgData ))
        {
            qDebug("Using reduced RAW picture extraction");
            return true;
        }
    }

    return false;
}

bool KDcraw::rawFileIdentify(DcrawInfoContainer& identify, const QString& path)
{
    FILE       *f=NULL;
    QByteArray  txtData;
    const int   MAX_IPC_SIZE = (1024*32);
    char        buffer[MAX_IPC_SIZE];
    QFile       file;
    Q_LONG      len;
    QCString    command;

    QFileInfo fileInfo(path);
    QString   rawFilesExt(raw_file_extentions);
    QString ext = fileInfo.extension(false).upper();

    if (!fileInfo.exists() || ext.isEmpty() || !rawFilesExt.upper().contains(ext))
        return false;

    // Try to get camera maker/model using dcraw with options:
    // -i : identify files without decoding them.
    // -v : verbose mode.

    command  = DcrawBinary::path();
    command += " -i -v ";
    command += QFile::encodeName( KProcess::quote( path ) );
    qDebug("Running RAW decoding command: %s", (const char*)command);

    f = popen( command.data(), "r" );

    if ( f == NULL )
    {
        identify = DcrawInfoContainer();
        return false;
    }

    file.open( IO_ReadOnly,  f );

    while ((len = file.readBlock(buffer, MAX_IPC_SIZE)) != 0)
    {
        if ( len == -1 )
        {
            identify = DcrawInfoContainer();
            return false;
        }
        else
        {
            int oldSize = txtData.size();
            txtData.resize( txtData.size() + len );
            memcpy(txtData.data()+oldSize, buffer, len);
        }
    }

    file.close();
    pclose( f );
    QString dcrawInfo(txtData);

    if ( dcrawInfo.isEmpty() )
    {
        identify = DcrawInfoContainer();
        return false;
    }

    int pos;

    // Extract Time Stamp.
    QString timeStampHeader("Timestamp: ");
    pos = dcrawInfo.find(timeStampHeader);
    if (pos != -1)
    {
        QString timeStamp = dcrawInfo.mid(pos).section('\n', 0, 0);
        timeStamp.remove(0, timeStampHeader.length());
        identify.dateTime = QDateTime::fromString(timeStamp);
    }

    // Extract Camera Maker.
    QString makeHeader("Camera: ");
    pos = dcrawInfo.find(makeHeader);
    if (pos != -1)
    {
        QString make = dcrawInfo.mid(pos).section('\n', 0, 0);
        make.remove(0, makeHeader.length());
        identify.make = make;
    }

    // Extract Camera Model.
    QString modelHeader("Model: ");
    pos = dcrawInfo.find(modelHeader);
    if (pos != -1)
    {
        QString model = dcrawInfo.mid(pos).section('\n', 0, 0);
        model.remove(0, modelHeader.length());
        identify.model = model;
    }

    // Extract Picture Owner.
    QString ownerHeader("Owner: ");
    pos = dcrawInfo.find(ownerHeader);
    if (pos != -1)
    {
        QString owner = dcrawInfo.mid(pos).section('\n', 0, 0);
        owner.remove(0, ownerHeader.length());
        identify.owner = owner;
    }

    // Extract DNG Version.
    QString DNGVersionHeader("DNG Version: ");
    pos = dcrawInfo.find(DNGVersionHeader);
    if (pos != -1)
    {
        QString DNGVersion = dcrawInfo.mid(pos).section('\n', 0, 0);
        DNGVersion.remove(0, DNGVersionHeader.length());
        identify.DNGVersion = DNGVersion;
    }

    // Extract ISO Speed.
    QString isoSpeedHeader("ISO speed: ");
    pos = dcrawInfo.find(isoSpeedHeader);
    if (pos != -1)
    {
        QString isoSpeed = dcrawInfo.mid(pos).section('\n', 0, 0);
        isoSpeed.remove(0, isoSpeedHeader.length());
        identify.sensitivity = isoSpeed.toLong();
    }

    // Extract Shutter Speed.
    QString shutterSpeedHeader("Shutter: ");
    pos = dcrawInfo.find(shutterSpeedHeader);
    if (pos != -1)
    {
        QString shutterSpeed = dcrawInfo.mid(pos).section('\n', 0, 0);
        shutterSpeed.remove(0, shutterSpeedHeader.length());

        if (shutterSpeed.startsWith("1/"))
            shutterSpeed.remove(0, 2);                   // remove "1/" at start of string.

        shutterSpeed.remove(shutterSpeed.length()-4, 4); // remove " sec" at end of string.
        identify.exposureTime = shutterSpeed.toFloat();
    }

    // Extract Aperture.
    QString apertureHeader("Aperture: f/");
    pos = dcrawInfo.find(apertureHeader);
    if (pos != -1)
    {
        QString aperture = dcrawInfo.mid(pos).section('\n', 0, 0);
        aperture.remove(0, apertureHeader.length());
        identify.aperture = aperture.toFloat();
    }

    // Extract Focal Length.
    QString focalLengthHeader("Focal Length: ");
    pos = dcrawInfo.find(focalLengthHeader);
    if (pos != -1)
    {
        QString focalLength = dcrawInfo.mid(pos).section('\n', 0, 0);
        focalLength.remove(0, focalLengthHeader.length());
        focalLength.remove(focalLength.length()-3, 3);    // remove " mm" at end of string.
        identify.focalLength = focalLength.toFloat();
    }

    // Extract Image Size.

    QString imageSizeHeader("Image size:  ");
    pos = dcrawInfo.find(imageSizeHeader);
    if (pos != -1)
    {
        QString imageSize = dcrawInfo.mid(pos).section('\n', 0, 0);
        imageSize.remove(0, imageSizeHeader.length());
        int width  = imageSize.section(" x ", 0, 0).toInt();
        int height = imageSize.section(" x ", 1, 1).toInt();
        identify.imageSize = QSize(width, height);
    }

    // Extract "Has an embedded ICC profile" flag.

    QString hasIccProfileHeader("Embedded ICC profile: ");
    pos = dcrawInfo.find(hasIccProfileHeader);
    if (pos != -1)
    {
        QString hasIccProfile = dcrawInfo.mid(pos).section('\n', 0, 0);
        hasIccProfile.remove(0, hasIccProfileHeader.length());
        if (hasIccProfile.contains("yes"))
            identify.hasIccProfile = true;
        else
            identify.hasIccProfile = false;
    }

    // Check if picture is decodable.

    identify.isDecodable = true;
    pos = dcrawInfo.find("Cannot decode file");
    if (pos != -1)
        identify.isDecodable = false;

    // Extract "Has Secondary Pixel" flag.

    QString hasSecondaryPixelHeader("Secondary pixels: ");
    pos = dcrawInfo.find(hasSecondaryPixelHeader);
    if (pos != -1)
    {
        QString hasSecondaryPixel = dcrawInfo.mid(pos).section('\n', 0, 0);
        hasSecondaryPixel.remove(0, hasSecondaryPixelHeader.length());
        if (hasSecondaryPixel.contains("yes"))
            identify.hasSecondaryPixel = true;
        else
            identify.hasSecondaryPixel = false;
    }

    // Extract Pixel Aspect Ratio.
    QString aspectRatioHeader("Pixel Aspect Ratio: ");
    pos = dcrawInfo.find(aspectRatioHeader);
    if (pos != -1)
    {
        QString aspectRatio = dcrawInfo.mid(pos).section('\n', 0, 0);
        aspectRatio.remove(0, aspectRatioHeader.length());
        identify.pixelAspectRatio = aspectRatio.toFloat();
    }

    // Extract Raw Colors.
    QString rawColorsHeader("Raw colors: ");
    pos = dcrawInfo.find(rawColorsHeader);
    if (pos != -1)
    {
        QString rawColors = dcrawInfo.mid(pos).section('\n', 0, 0);
        rawColors.remove(0, rawColorsHeader.length());
        identify.rawColors = rawColors.toInt();
    }

    // Extract Filter Pattern.
    QString filterHeader("Filter pattern: ");
    pos = dcrawInfo.find(filterHeader);
    if (pos != -1)
    {
        QString filter = dcrawInfo.mid(pos).section('\n', 0, 0);
        filter.remove(0, filterHeader.length());
        identify.filterPattern = filter;
    }

    // Extract Daylight Multipliers.
    QString daylightMultHeader("Daylight multipliers: ");
    pos = dcrawInfo.find(daylightMultHeader);
    if (pos != -1)
    {
        QString daylightMult = dcrawInfo.mid(pos).section('\n', 0, 0);
        daylightMult.remove(0, daylightMultHeader.length());
        identify.daylightMult[0] = daylightMult.section(" ", 0, 0).toDouble();
        identify.daylightMult[1] = daylightMult.section(" ", 1, 1).toDouble();
        identify.daylightMult[2] = daylightMult.section(" ", 2, 2).toDouble();
    }

    // Extract Camera Multipliers.
    QString cameraMultHeader("Camera multipliers: ");
    pos = dcrawInfo.find(cameraMultHeader);
    if (pos != -1)
    {
        QString cameraMult = dcrawInfo.mid(pos).section('\n', 0, 0);
        cameraMult.remove(0, cameraMultHeader.length());
        identify.cameraMult[0] = cameraMult.section(" ", 0, 0).toDouble();
        identify.cameraMult[1] = cameraMult.section(" ", 1, 1).toDouble();
        identify.cameraMult[2] = cameraMult.section(" ", 2, 2).toDouble();
        identify.cameraMult[3] = cameraMult.section(" ", 3, 3).toDouble();
    }

    return true;
}

// ----------------------------------------------------------------------------------

bool KDcraw::decodeHalfRAWImage(const QString& filePath, RawDecodingSettings rawDecodingSettings, 
                                QByteArray &imageData, int &width, int &height, int &rgbmax)
{
    m_rawDecodingSettings                    = rawDecodingSettings;
    m_rawDecodingSettings.halfSizeColorImage = true;
    return (loadFromDcraw(filePath, imageData, width, height, rgbmax));
}

bool KDcraw::decodeRAWImage(const QString& filePath, RawDecodingSettings rawDecodingSettings, 
                            QByteArray &imageData, int &width, int &height, int &rgbmax)
{
    m_rawDecodingSettings = rawDecodingSettings;
    return (loadFromDcraw(filePath, imageData, width, height, rgbmax));
}

bool KDcraw::checkToCancelWaitingData()
{
    return m_cancel;
}

bool KDcraw::checkToCancelRecievingData()
{
    return m_cancel;
}

void KDcraw::setWaitingDataProgress(double)
{
}

void KDcraw::setRecievingDataProgress(double)
{
}

bool KDcraw::loadFromDcraw(const QString& filePath, QByteArray &imageData, 
                           int &width, int &height, int &rgbmax)
{
    m_cancel      = false;
    d->dataPos     = 0;
    d->filePath   = filePath;
    d->running    = true;
    d->normalExit = false;
    d->process    = 0;
    d->data       = 0;
    d->width      = 0;
    d->height     = 0;
    d->rgbmax     = 0;

    // trigger startProcess and loop to wait dcraw decoding
    QApplication::postEvent(this, new QCustomEvent(QEvent::User));

    // The time from starting dcraw to when it first outputs something takes
    // much longer than the time while it outputs the data and the time while
    // we process the data.
    // We do not have progress information for this, but it is much more promising to the user
    // if there is progress which does not stay at a fixed value.
    // So we make up some progress (0% - 40%), using the file size as an indicator how long it might take.
    QTime dcrawStartTime = QTime::currentTime();
    int fileSize         = QFileInfo(filePath).size();
    
    // This is the magic number that describes how fast the function grows
    // It _should_ be dependent on how fast the computer is, but we do not have this piece of information
    // So this is a number that works well on my computer.
    double K50         = 3000.0*fileSize;
    double part        = 0;
    int checkpointTime = 0;
    int checkpoint     = 0;

    // The shuttingDown is a hack needed to prevent hanging when this KProcess-based loader
    // is waiting for the process to finish, but the main thread is waiting
    // for the thread to finish and no KProcess events are delivered.
    // Remove when porting to Qt4.
    while (d->running && !checkToCancelRecievingData())
    {
        if (d->dataPos == 0)
        {
            int elapsedMsecs = dcrawStartTime.msecsTo(QTime::currentTime());
            if (elapsedMsecs > checkpointTime)
                checkpointTime += 300;
    
            // What we do here is a sigmoidal curve, it starts slowly,
            // then grows more rapidly, slows down again and
            // get asymptotically closer to the maximum.
            // (this is the Hill Equation, 2.8 the Hill Coefficient, to pour some blood in this)
            double elapsedMsecsPow = pow(elapsedMsecs, 2.8);
            part = (elapsedMsecsPow) / (K50 + elapsedMsecsPow);

            // While we waiting to receive data, progress from 0% to 40%
            setWaitingDataProgress(0.4*part);
        }
        else if (d->dataPos > checkpoint)
        {
            // While receiving data, progress from 40% to 70%
            double delta = 0.3 + 0.4 - 0.4*part;
            int imageSize = d->width * d->height * (m_rawDecodingSettings.sixteenBitsImage ? 6 : 3);
            checkpoint += (int)(imageSize / (20 * delta));
            setRecievingDataProgress(0.4*part + delta * (((float)d->dataPos)/((float)imageSize))); 
        }

        QMutexLocker lock(&d->mutex);
        d->condVar.wait(&d->mutex, 10);
    }

    if (!d->normalExit || m_cancel)
    {
        delete [] d->data;
        d->data = 0;
        return false;
    }

    // Copy decoded image data to byte array.
    width     = d->width;
    height    = d->height;
    rgbmax    = d->rgbmax;
    imageData = QByteArray(d->width * d->height * (m_rawDecodingSettings.sixteenBitsImage ? 6 : 3));
    memcpy(imageData.data(), d->data, imageData.size());

    delete [] d->data;
    d->data = 0;

    return true;
}

void KDcraw::customEvent(QCustomEvent *)
{
    // KProcess (because of QSocketNotifier) is not reentrant.
    // We must only use it from the main thread.
    startProcess();

    // set up timer to call continueQuery at regular intervals
    if (d->running)
    {
        d->queryTimer = new QTimer;
        connect(d->queryTimer, SIGNAL(timeout()),
                this, SLOT(slotContinueQuery()));
        d->queryTimer->start(30);
    }
}

void KDcraw::slotContinueQuery()
{
    // this is called from the timer

    if (checkToCancelWaitingData())
    {
        d->process->kill();
    }
}

void KDcraw::startProcess()
{
    if (m_cancel)
    {
        d->running    = false;
        d->normalExit = false;
        return;
    }

    // create KProcess and build argument list

    d->process = new KProcess;

    connect(d->process, SIGNAL(processExited(KProcess *)),
            this, SLOT(slotProcessExited(KProcess *)));
             
    connect(d->process, SIGNAL(receivedStdout(KProcess *, char *, int)),
            this, SLOT(slotReceivedStdout(KProcess *, char *, int)));
             
    connect(d->process, SIGNAL(receivedStderr(KProcess *, char *, int)),
            this, SLOT(slotReceivedStderr(KProcess *, char *, int)));

    // run dcraw with options:
    // -c : write to stdout
    // -v : verboze mode.
    //
    // -4 : 16bit ppm output
    //
    // -f : Interpolate RGB as four colors. This blurs the image a little, but it eliminates false 2x2 mesh patterns.
    // -a : Use automatic white balance
    // -w : Use camera white balance, if possible
    // -n : Use wavelets to erase noise while preserving real detail. 
    // -j : Do not stretch the image to its correct aspect ratio.
    // -q : Use an interpolation method.
    // -p : Use the input ICC profiles to define the camera's raw colorspace.
    // -o : Use ICC profiles to define the output colorspace.
    // -h : Output a half-size color image. Twice as fast as -q 0.
    // -b : set Brightness value.
    // -k : set Black Point value.
    // -r : set Raw Color Balance Multipliers.

    *d->process << DcrawBinary::path();
    *d->process << "-c";
    *d->process << "-v";

    if (m_rawDecodingSettings.sixteenBitsImage)
        *d->process << "-4";

    if (m_rawDecodingSettings.halfSizeColorImage)
        *d->process << "-h";

    if (m_rawDecodingSettings.cameraColorBalance)
        *d->process << "-w";

    if (m_rawDecodingSettings.automaticColorBalance)
        *d->process << "-a";

    if (m_rawDecodingSettings.RGBInterpolate4Colors)
        *d->process << "-f";

    if (m_rawDecodingSettings.DontStretchPixels)
        *d->process << "-j";

    *d->process << "-H";
    *d->process << QString::number(m_rawDecodingSettings.unclipColors);

    *d->process << "-b";
    *d->process << QString::number(m_rawDecodingSettings.brightness);

    if (m_rawDecodingSettings.enableBlackPoint)
    {
        *d->process << "-k";
        *d->process << QString::number(m_rawDecodingSettings.blackPoint);
    }

    if (m_rawDecodingSettings.enableColorMultipliers)
    {
        *d->process << "-r";
        *d->process << QString::number(m_rawDecodingSettings.colorBalanceMultipliers[0], 'f', 5);
        *d->process << QString::number(m_rawDecodingSettings.colorBalanceMultipliers[1], 'f', 5);
        *d->process << QString::number(m_rawDecodingSettings.colorBalanceMultipliers[2], 'f', 5);
        *d->process << QString::number(m_rawDecodingSettings.colorBalanceMultipliers[3], 'f', 5);
    }

    *d->process << "-q";
    *d->process << QString::number(m_rawDecodingSettings.RAWQuality);

    if (m_rawDecodingSettings.enableNoiseReduction)
    {
        *d->process << "-n";
        *d->process << QString::number(m_rawDecodingSettings.NRThreshold);
    }

    if (m_rawDecodingSettings.enableCACorrection)
    {
        *d->process << "-C";
        *d->process << QString::number(m_rawDecodingSettings.caMultiplier[0], 'f', 5);
        *d->process << QString::number(m_rawDecodingSettings.caMultiplier[1], 'f', 5);
    }

    *d->process << "-o";
    *d->process << QString::number(m_rawDecodingSettings.outputColorSpace);

    *d->process << QFile::encodeName(d->filePath);

    QString args;
    for (uint i = 0 ; i < d->process->args().count(); i++)
    {
        args.append(d->process->args()[i]);
        args.append(QString(" "));
    }

    qDebug("Running RAW decoding command: %s", args.ascii());

    // actually start the process
    if ( !d->process->start(KProcess::NotifyOnExit, 
         KProcess::Communication(KProcess::Stdout | KProcess::Stderr)) )
    {
        qWarning("Failed to start RAW decoding");
        delete d->process;
        d->process    = 0;
        d->running    = false;
        d->normalExit = false;
        return;
    }
}

void KDcraw::slotProcessExited(KProcess *)
{
    // set variables, clean up, wake up loader thread

    QMutexLocker lock(&d->mutex);
    d->running    = false;
    d->normalExit = d->process->normalExit() && d->process->exitStatus() == 0;
    delete d->process;
    d->process    = 0;
    delete d->queryTimer;
    d->queryTimer = 0;
    d->condVar.wakeAll();
}

void KDcraw::slotReceivedStdout(KProcess *, char *buffer, int buflen)
{
    if (!d->data)
    {
        // first data packet:
        // Parse PPM header to find out size and allocate buffer

        // PPM header is "P6 <width> <height> <maximum rgb value "
        // where the blanks are newline characters

        QString magic = QString::fromAscii(buffer, 2);
        if (magic != "P6") 
        {
            qWarning("Cannot parse header from RAW decoding: Magic is: %s", magic.ascii());
            d->process->kill();
            return;
        }

        // Find the third newline that marks the header end in a dcraw generated ppm.
        int i       = 0;
        int counter = 0;

        while (i < buflen) 
        {
            if (counter == 3) break;
            if (buffer[i] == '\n') 
            {
                counter++;
            }
            ++i;
        }

        QStringList splitlist = QStringList::split("\n", QString::fromAscii(buffer, i));
        QStringList sizes     = QStringList::split(" ", splitlist[1]);
        if (splitlist.size() < 3 || sizes.size() < 2)
        {
            qWarning("Cannot parse header from RAW decoding: Could not split");
            d->process->kill();
            return;
        }

        d->width  = sizes[0].toInt();
        d->height = sizes[1].toInt();
        d->rgbmax = splitlist[2].toInt();

#ifdef ENABLE_DEBUG_MESSAGES
        qDebug("Parsed PPM header: width %i height %i rgbmax %i", d->width, d->height, d->rgbmax);
#endif

        // cut header from data for memcpy below
        buffer += i;
        buflen -= i;

        // allocate buffer
        d->data    = new uchar[d->width * d->height * (m_rawDecodingSettings.sixteenBitsImage ? 6 : 3)];
        d->dataPos = 0;
    }

    // copy data to buffer
    memcpy(d->data + d->dataPos, buffer, buflen);
    d->dataPos += buflen;
}

void KDcraw::slotReceivedStderr(KProcess *, char *buffer, int buflen)
{
    QCString message(buffer, buflen);
    qDebug("RAW decoding StdErr: %s", (const char*)message);
}

}  // namespace KDcrawIface
