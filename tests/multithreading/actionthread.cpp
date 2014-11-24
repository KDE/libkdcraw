/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  : 2014-10-17
 * @brief : a class to manage Raw to Png conversion using threads
 *
 * @author Copyright (C) 2011-2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2014 by Veaceslav Munteanu
 *         <a href="mailto:veaceslav dot munteanu90 at gmail dot com">veaceslav dot munteanu90 at gmail dot com</a>
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

#include "actionthread.h"

// Qt includes

#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>
#include <QFileInfo>
#include <QImage>
#include <QDebug>

// Local includes

#include "kdcraw.h"
#include "rawdecodingsettings.h"
#include "ractionjob.h"

class ActionThread::Task : public RActionJob
{
public:

    Task(QObject* const parent = 0)
        : RActionJob()
    {
        Q_UNUSED(parent);
    }

    QString errString;
    QUrl    fileUrl;

protected:

    void run()
    {
        emit signalStarted();

        // RAW to PNG
        QImage              image;
        KDcraw              rawProcessor;
        
        emit signalProgress(20);
        
        RawDecodingSettings settings;
        settings.halfSizeColorImage    = false;
        settings.sixteenBitsImage      = false;
        settings.RGBInterpolate4Colors = false;
        settings.RAWQuality            = RawDecodingSettings::BILINEAR;
        
        emit signalProgress(30);
        
        QFileInfo input(fileUrl.toLocalFile());
        QString   fullFilePath(input.baseName() + QString(".full.png"));
        QFileInfo fullOutput(fullFilePath);

        emit signalProgress(40);

        if (!rawProcessor.loadFullImage(image, fileUrl.toLocalFile(), settings))
        {
            errString = "raw2png: Loading full RAW image failed. Aborted...";
            return;
        }

        emit signalProgress(60);

        qDebug() << "raw2png: Saving full RAW image to "
                 << fullOutput.fileName() << " size ("
                 << image.width() << "x" << image.height()
                 << ")";

        emit signalProgress(80);

        image.save(fullFilePath, "PNG");

        emit signalDone();
    }
};

// ----------------------------------------------------------------------------------------------------

ActionThread::ActionThread(QObject* const parent)
    : RActionThreadBase(parent)
{
}

ActionThread::~ActionThread()
{
}

void ActionThread::convertRAWtoPNG(const QList<QUrl>& list)
{
    RJobCollection collection;

    foreach (const QUrl& url, list)
    {
        Task* const job = new Task(this);
        job->fileUrl    = url;

        connect(job, SIGNAL(signalStarted()),
                this, SLOT(slotJobStarted()));

        connect(job, SIGNAL(signalProgress(int)),
                this, SLOT(slotJobProgress(int)));

        connect(job, SIGNAL(signalDone()),
                this, SLOT(slotJobDone()));

        collection.append(job);

        qDebug() << "Appending file to process " << url;
    }

    appendJobs(collection);
}

void ActionThread::slotJobDone()
{
    Task* const task = dynamic_cast<Task*>(sender());
    if (!task) return;

    if (task->errString.isEmpty())
    {
        emit finished(task->fileUrl);
    }
    else
    {
        emit failed(task->fileUrl, task->errString);
    }
}

void ActionThread::slotJobProgress(int p)
{
    Task* const task = dynamic_cast<Task*>(sender());
    if (!task) return;

    emit progress(task->fileUrl, p);
}

void ActionThread::slotJobStarted()
{
    Task* const task = dynamic_cast<Task*>(sender());
    if (!task) return;

    emit starting(task->fileUrl);
}