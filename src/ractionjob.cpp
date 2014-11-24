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

#include "ractionjob.h"

// Qt includes

#include <QDebug>

namespace KDcrawIface
{

RActionJob::RActionJob()
    : QObject(),
      QRunnable(),
      m_cancel(false)
{
}

RActionJob::~RActionJob()
{
}

void RActionJob::cancel()
{
    m_cancel = true;
}

} // namespace KDcrawIface
