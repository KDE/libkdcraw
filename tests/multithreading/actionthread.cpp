/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * Date        : 2014-10-17
 * Description : a class to manage Raw to Png conversion using threads
 *
 * Copyright (C) 2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

// KDE includes

#include <kdebug.h>
#include <ThreadWeaver/ThreadWeaver>

// Local includes

#include "enhancedjob.h"

//#include <ThreadWeaver/JobCollection.h>

// Local includes

#include "kdcraw.h"
#include "rawdecodingsettings.h"
#include "jobcollectionz.h"

using namespace ThreadWeaver;

class ActionThread::Task : public EnhancedJob
{
public:

    Task(QObject* const parent = 0)
        :EnhancedJob()
    {
        Q_UNUSED(parent);
    }

    QString errString;
    QUrl    fileUrl;

protected:

    void run(JobPointer self, Thread *thread)
    {
        Q_UNUSED(self);
        Q_UNUSED(thread);
        // RAW to PNG
        QImage              image;
        KDcraw              rawProcessor;
        RawDecodingSettings settings;
        settings.halfSizeColorImage    = false;
        settings.sixteenBitsImage      = false;
        settings.RGBInterpolate4Colors = false;
        settings.RAWQuality            = RawDecodingSettings::BILINEAR;

        QFileInfo input(fileUrl.toLocalFile());
        QString   fullFilePath(input.baseName() + QString(".full.png"));
        QFileInfo fullOutput(fullFilePath);

        if (!rawProcessor.loadFullImage(image, fileUrl.toLocalFile(), settings))
        {
            errString = "raw2png: Loading full RAW image failed. Aborted...";
            return;
        }

        qDebug() << "raw2png: Saving full RAW image to "
                 << fullOutput.fileName() << " size ("
                 << image.width() << "x" << image.height()
                 << ")";
        image.save(fullFilePath, "PNG");

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

void ActionThread::convertRAWtoPNG(const QList<QUrl> &list)
{
    JobCollectionz* const collection = new JobCollectionz();

    for (QList<QUrl>::const_iterator it = list.constBegin(); it != list.constEnd(); ++it )
    {
        Task* const t = new Task(this);
        t->fileUrl    = *it;
        qDebug() << "Appending new work " << *it;

//        connect(t, SIGNAL(started(ThreadWeaver::Job*)),
//                this, SLOT(slotJobStarted(ThreadWeaver::Job*)));

        connect(t, SIGNAL(signalDone(ThreadWeaver::Job*)),
                this, SLOT(slotJobDone(ThreadWeaver::Job*)));

        collection->addJob((JobPointer)t);
    }

    appendJob(collection);
}

void ActionThread::slotJobDone(ThreadWeaver::Job* job)
{
    Task* const task = static_cast<Task*>(job);

    if (task->errString.isEmpty())
    {
        kDebug() << "Job done:" << task->fileUrl.toLocalFile();
        emit finished(task->fileUrl);
    }
    else
    {
        kDebug() << "Could not complete the job: " << task->fileUrl.toLocalFile() << " Error: " << task->errString;
        emit failed(task->fileUrl, task->errString);
    }

    delete job;
}

void ActionThread::slotJobStarted(ThreadWeaver::Job* job)
{
    Task* const task = static_cast<Task*>(job);
    kDebug() << "Job Started:" << task->fileUrl.toLocalFile();
    emit starting(task->fileUrl);
}
