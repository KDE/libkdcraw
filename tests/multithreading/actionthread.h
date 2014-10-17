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

#ifndef ACTIONTHREAD_H
#define ACTIONTHREAD_H

// Qt includes

#include <QThread>

// KDE includes

#include <kurl.h>

// Libkdcraw includes

#include "ractionthreadbase.h"

namespace ThreadWeaver
{
    class Job;
}

using namespace KDcrawIface;

class ActionThread : public RActionThreadBase
{
    Q_OBJECT

public:

    ActionThread(QObject* const parent);
    ~ActionThread();

    void convertRAWtoPNG(const KUrl::List& list);

Q_SIGNALS:

    void starting(const KUrl& url);
    void finished(const KUrl& url);
    void failed(const KUrl& url, const QString& err);

private Q_SLOTS:

    void slotJobDone(ThreadWeaver::Job*);
    void slotJobStarted(ThreadWeaver::Job*);

private:

    class Task;
};

#endif /* ACTIONTHREAD_H */
