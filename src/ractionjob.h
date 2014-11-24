/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2014-15-11
 * @brief  QRunnable job extended with QObject features
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

#ifndef RACTIONJOB_H
#define RACTIONJOB_H

// Qt includes

#include <QObject>
#include <QRunnable>

// Local includes

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class LIBKDCRAW_EXPORT RActionJob : public QObject, public QRunnable
{
    Q_OBJECT

public:

    RActionJob();
    virtual ~RActionJob();

Q_SIGNALS:

    void signalStarted();
    void signalProgress(int);
    void signalDone();

public Q_SLOTS:

    void cancel();

protected:

    bool m_cancel;
};

typedef QList<RActionJob*> RJobCollection;

} // namespace KDcrawIface

#endif // RACTIONJOB_H
