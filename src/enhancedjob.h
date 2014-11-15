/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2014-15-11
 * @brief  ThreadWeaver job extended with QObject features
 *
 * @author Copyright (C) 2014 by Veaceslav Munteanu
 *         veaceslav dot munteanu90 at gmail dot com
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
#ifndef ENHANCEDJOB_H
#define ENHANCEDJOB_H
#include "ThreadWeaver/Job"
#include "kdcraw_export.h"
namespace KDcrawIface
{

class LIBKDCRAW_EXPORT EnhancedJob : public QObject, public ThreadWeaver::Job
{
    Q_OBJECT
public:
    EnhancedJob();
    virtual ~EnhancedJob();

protected:
    void defaultEnd(const ThreadWeaver::JobPointer &job,
                    ThreadWeaver::Thread *thread);
signals:
    void done(Job*);
};
}

#endif
