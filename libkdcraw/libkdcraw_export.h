/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2005-09-15
 * Description : dcraw program interface for KDE
 *
 * Copyright (C) 2005 by Laurent Montel <montel@kde.org>
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

#ifndef _LIBKDCRAW_EXPORT_H
#define _LIBKDCRAW_EXPORT_H

#ifdef KDEMACROS_USABLE
#include <kdemacros.h>
#endif

#ifdef KDE_EXPORT
#define LIBKDCRAW_EXPORT KDE_EXPORT
#else
#define LIBKDCRAW_EXPORT
#endif

#endif /* _LIBKDCRAW_EXPORT_H */
