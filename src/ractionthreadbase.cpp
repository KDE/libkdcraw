/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2011-12-28
 * @brief  re-implementation of action thread using threadweaver
 *
 * @author Copyright (C) 2011-2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2014 by Veaceslav Munteanu
 *         <a href="mailto:veaceslav dot munteanu90 at gmail dot com">veaceslav dot munteanu90 at gmail dot com</a>
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
#include <QObject>
#include <QDebug>
#include <QWaitCondition>
#include <QMutex>
#include <QList>
#include <QThreadPool>

namespace KDcrawIface
{
    
class RActionThreadBase::Private
{
public:

    Private()
    {
        running = false;
        pool    = QThreadPool::globalInstance();
    }

    volatile bool  running;

    QWaitCondition condVarJobs;
    QMutex         mutex;
    RJobCollection todo;
    RJobCollection pending;

    QThreadPool*   pool;
};

RActionThreadBase::RActionThreadBase(QObject* const parent)
    : QThread(parent),
      d(new Private)
{
    const int maximumNumberOfThreads = qMax(QThreadPool::globalInstance()->maxThreadCount(), 1);
    setMaximumNumberOfThreads(maximumNumberOfThreads);
    qDebug() << "Starting Main Thread";
}

RActionThreadBase::~RActionThreadBase()
{
    qDebug() << "calling action thread destructor";

    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();

    delete d;
}

void RActionThreadBase::setMaximumNumberOfThreads(int n)
{
    d->pool->setMaxThreadCount(n);
    qDebug() << "Using " << n << " CPU core to run threads";
}

int RActionThreadBase::maximumNumberOfThreads() const
{
    return d->pool->maxThreadCount();
}

void RActionThreadBase::slotJobFinished(RActionJob* job)
{
    qDebug() << "One job is done";

    QMutexLocker lock(&d->mutex);
    d->pending.removeOne(job);
    d->condVarJobs.wakeAll();

    if (isEmpty())
    {
        d->running = false;
    }
}

void RActionThreadBase::cancel()
{
    qDebug() << "Cancel Main Thread";
    QMutexLocker lock(&d->mutex);

    d->todo.clear();

    foreach(RActionJob* const job, d->pending)
    {
        job->cancel();
    }

    d->pending.clear();
    d->condVarJobs.wakeAll();
    d->running = false;
}

bool RActionThreadBase::isEmpty() const
{
    return d->pending.isEmpty();
}

void RActionThreadBase::appendJobs(const RJobCollection& jobs)
{
    QMutexLocker lock(&d->mutex);
    d->todo << jobs;
    d->condVarJobs.wakeAll();
}

void RActionThreadBase::run()
{
    d->running = true;

    while (d->running)
    {
        QMutexLocker lock(&d->mutex);

        if (!d->todo.isEmpty())
        {
            qDebug() << "Action Thread run " << d->todo.count() << " new jobs";

            foreach(RActionJob* const job, d->todo)
            {

                connect(job, SIGNAL(signalDone(RActionJob*)),
                        this, SLOT(slotJobFinished(RActionJob*)));

                d->pool->start(job);
                d->pending << job;
            }
            
            d->todo.clear();
        }
        else
        {
            d->condVarJobs.wait(&d->mutex);
        }
    }
}

}  // namespace KDcrawIface
