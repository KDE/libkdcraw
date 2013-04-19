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

#ifndef RACTION_THREAD_BASE_H
#define RACTION_THREAD_BASE_H

// Qt includes

#include <QtCore/QThread>

// Local includes

#include "libkdcraw_export.h"

namespace ThreadWeaver
{
    class JobCollection;
}

using namespace ThreadWeaver;

namespace KDcrawIface
{

class LIBKDCRAW_EXPORT RActionThreadBase : public QThread
{
    Q_OBJECT

public:

    RActionThreadBase(QObject* const parent=0);
    ~RActionThreadBase();

    /** Adjust maximum number of thread used to parallelize collection of job processing.
     *  By default in constructor, KDE::Solid API is used to determine the number of CPU available and adjust
     *  this value automatically.
     */
    void setMaximumNumberOfThreads(int n);

    void cancel();
    void finish();

protected:

    void run();

    /** Append a collection of jobs to process in pending list.
     */
    void appendJob(JobCollection* const job);

    /** Return true if list of pending jobs to process is empty.
     */
    bool isEmpty() const;

protected Q_SLOTS:

    void slotFinished();

private:

    class Private;
    Private* const d;
};

}  // namespace KDcrawIface

#endif // RACTION_THREAD_BASE_H
