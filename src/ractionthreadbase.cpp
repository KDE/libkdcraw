/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2011-12-28
 * @brief  re-implementation of action thread using threadweaver
 *
 * @author Copyright (C) 2011-2013 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2011-2012 by A Janardhan Reddy
 *         <a href="annapareddyjanardhanreddy at gmail dot com">annapareddyjanardhanreddy at gmail dot com</a>
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

#include "ractionthreadbase.h"

// Qt includes

#include <QMutexLocker>

// KDE includes

#include <kdebug.h>
#include <ThreadWeaver/Weaver>
#include <ThreadWeaver/ThreadWeaver>
#include <ThreadWeaver/Job>
#include <ThreadWeaver/DebuggingAids>
#include <solid/device.h>

// Local includes

#include "ractionthreadbase_p.h"
#include "jobcollectionz.h"

using namespace Solid;
using namespace ThreadWeaver;

namespace KDcrawIface
{

RActionThreadBase::RActionThreadBase(QObject* const parent)
    : QThread(parent), d(new Private)
{
    const int maximumNumberOfThreads = qMax(Device::listFromType(DeviceInterface::Processor).count(), 1);
//    d->log                           = new RWeaverObserver(this);
    d->weaver                        = new Weaver(this);
//    d->weaver->registerObserver(d->log);
    d->weaver->setMaximumNumberOfThreads(maximumNumberOfThreads);
    kDebug() << "Starting Main Thread";
}

RActionThreadBase::~RActionThreadBase()
{
    kDebug() << "calling action thread destructor";
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

//    delete d->log;
    delete d->weaver;
    delete d;
}

void RActionThreadBase::setMaximumNumberOfThreads(int n)
{
    d->weaver->setMaximumNumberOfThreads(n);
}

void RActionThreadBase::slotFinished()
{
    kDebug() << "Finish Main Thread";
    d->weaverRunning = false;
    d->condVarJobs.wakeAll();
    // You can't emit with QPrivateSignal
//    emit QThread::finished(QThread::QPrivateSignal());
}

void RActionThreadBase::cancel()
{
    kDebug() << "Cancel Main Thread";
    QMutexLocker lock(&d->mutex);
    d->todo.clear();
    d->running       = false;
    d->weaverRunning = true;
    d->weaver->requestAbort();
    d->weaver->dequeue();
    d->condVarJobs.wakeAll();
}

void RActionThreadBase::finish()
{
    d->weaver->finish();
}

bool RActionThreadBase::isEmpty() const
{
    return d->todo.isEmpty();
}

void RActionThreadBase::appendJob(JobCollectionz * const job)
{
    QMutexLocker lock(&d->mutex);
    d->todo << job;
    d->condVarJobs.wakeAll();
}

void RActionThreadBase::run()
{
    d->running       = true;
    d->weaverRunning = false;
    kDebug() << "In action thread Run";

    while (d->running)
    {
        JobCollectionz* t = 0;
        {
            QMutexLocker lock(&d->mutex);

            if (!isEmpty() && !d->weaverRunning)
            {
                t = d->todo.takeFirst();
            }
            else
            {
                d->condVarJobs.wait(&d->mutex);
            }
        }

        if (t)
        {
            connect(t, SIGNAL(signalDone()), this, SLOT(slotFinished()));

            connect(t, SIGNAL(signalDone()), t, SLOT(deleteLater()));

            d->weaverRunning = true;
            d->weaver->enqueue(QVector<JobPointer>() << JobPointer(t));
        }
    }

    d->weaver->finish();
    kDebug() << "Exiting Action Thread";
}

}  // namespace KDcrawIface
