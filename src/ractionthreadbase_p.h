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

#ifndef RACTION_THREAD_BASE_P_H
#define RACTION_THREAD_BASE_P_H

// Qt includes

#include <QWaitCondition>
#include <QMutex>
#include <QList>

// KDE includes

#include <threadweaver/Job.h>
#include <threadweaver/WeaverObserver.h>
#include <threadweaver/State.h>
#include <threadweaver/Thread.h>

// Local includes

#include "ractionthreadbase.h"

namespace ThreadWeaver
{
    class Weaver;
}

using namespace ThreadWeaver;

namespace KDcrawIface
{

/** RWeaverObserver is a simple wrapper to plug on the ActionThread class to
    prints debug messages when signals are received.
*/
class RWeaverObserver : public WeaverObserver
{
    Q_OBJECT

public:

    RWeaverObserver(QObject* const parent=0);
    ~RWeaverObserver();

protected Q_SLOTS:

    void slotWeaverStateChanged(ThreadWeaver::State*);
    void slotThreadStarted(ThreadWeaver::Thread*);
    void slotThreadBusy(ThreadWeaver::Thread*, ThreadWeaver::Job*);
    void slotThreadSuspended(ThreadWeaver::Thread*);
    void slotThreadExited(ThreadWeaver::Thread*);
};

// ----------------------------------------------------------------------------------

class RActionThreadBase::Private
{
public:

    Private()
    {
        running       = false;
        weaverRunning = false;
        weaver        = 0;
        log           = 0;
    }

    volatile bool         running;
    volatile bool         weaverRunning;

    QWaitCondition        condVarJobs;
    QMutex                mutex;
    QList<JobCollection*> todo;

    Weaver*               weaver;
    RWeaverObserver*      log;
};

}  // namespace KDcrawIface

#endif // RACTION_THREAD_BASE_P_H
