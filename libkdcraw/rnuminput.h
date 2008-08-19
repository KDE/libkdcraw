/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-08-16
 * Description : Integer and double num input widget
 *               re-implemented with a reset button to switch to
 *               a default value.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef RNUMINPUT_H
#define RNUMINPUT_H

// KDE includes.

#include <khbox.h>
#include <knuminput.h>

// Local includes

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class RIntNumInputPriv;

class LIBKDCRAW_EXPORT RIntNumInput : public KHBox
{

Q_OBJECT

public:

    RIntNumInput(QWidget *parent=0);
    ~RIntNumInput();

    void setValue(int d);
    int  value();

    void setSliderEnabled(bool b);
    void setRange(int min, int max, int step);

    void setDefaultValue(int d);
    int  defaultValue();

    KIntNumInput* input() const;

Q_SIGNALS:

    void reset();
    void valueChanged(int);

public Q_SLOTS:

    void slotReset();

private Q_SLOTS:

    void slotValueChanged(int);

private:

    RIntNumInputPriv* d;
};

// ---------------------------------------------------------

class RDoubleNumInputPriv;

class LIBKDCRAW_EXPORT RDoubleNumInput : public KHBox
{

Q_OBJECT

public:

    RDoubleNumInput(QWidget *parent=0);
    ~RDoubleNumInput();

    void   setValue(double d);
    double value();

    void   setDecimals(int p);
    void   setRange(double min, double max, double step);

    void   setDefaultValue(double d);
    double defaultValue();

    KDoubleNumInput* input() const;

Q_SIGNALS:

    void reset();
    void valueChanged(double);

public Q_SLOTS:

    void slotReset();

private Q_SLOTS:

    void slotValueChanged(double);

private:

    RDoubleNumInputPriv* d;
};

}  // namespace KDcrawIface

#endif /* RNUMINPUT_H */
