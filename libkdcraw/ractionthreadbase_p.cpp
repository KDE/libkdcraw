/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2011-12-28
 * @brief  internal RActionThreadBase classes
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

#include "ractionthreadbase_p.moc"

// KDE includes

#include <kdebug.h>

namespace KDcrawIface
{

RWeaverObserver::RWeaverObserver(QObject* const parent)
    : WeaverObserver(parent)
{
    connect(this, SIGNAL(weaverStateChanged(ThreadWeaver::State*)),
            this, SLOT(slotWeaverStateChanged(ThreadWeaver::State*)));

    connect(this, SIGNAL(threadStarted(ThreadWeaver::Thread*)),
            this, SLOT(slotThreadStarted(ThreadWeaver::Thread*)));

    connect(this, SIGNAL(threadBusy(ThreadWeaver::Thread*,ThreadWeaver::Job*)),
            this, SLOT(slotThreadBusy(ThreadWeaver::Thread*,ThreadWeaver::Job*)));

    connect(this, SIGNAL(threadSuspended(ThreadWeaver::Thread*)),
            this, SLOT(slotThreadSuspended(ThreadWeaver::Thread*)));

    connect(this, SIGNAL(threadExited(ThreadWeaver::Thread*)),
            this, SLOT(slotThreadExited(ThreadWeaver::Thread*)));
}

RWeaverObserver::~RWeaverObserver()
{
}

void RWeaverObserver::slotWeaverStateChanged(State* state)
{
    kDebug() << "RWeaverObserver: thread state changed to " << state->stateName();
}

void RWeaverObserver::slotThreadStarted(Thread* th)
{
    kDebug() << "RWeaverObserver: thread " << th->id()  <<" started";
}

void RWeaverObserver::slotThreadBusy(Thread* th, Job*)
{
    kDebug() << "RWeaverObserver: thread " << th->id()  << " busy";
}

void RWeaverObserver::slotThreadSuspended(Thread* th )
{
    kDebug() << "RWeaverObserver: thread " << th->id()  << " suspended";
}

void RWeaverObserver::slotThreadExited(Thread* th)
{
    kDebug() << "RWeaverObserver: thread " << th->id()  << " exited";
}

}  // namespace KDcrawIface
