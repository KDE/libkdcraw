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

// Qt includes.

#include <qhbox.h>

// KDE includes.

#include <knuminput.h>

// Local includes.

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class RIntNumInputPriv;

class LIBKDCRAW_EXPORT RIntNumInput : public QHBox
{

Q_OBJECT

public:

    RIntNumInput(QWidget *parent=0);
    ~RIntNumInput();

    int  value();

    void setRange(int min, int max, int step);

    void setDefaultValue(int d);
    int  defaultValue();

    KIntNumInput* input() const;

signals:

    void reset();
    void valueChanged(int);

public slots:

    void setValue(int d);
    void slotReset();

private slots:

    void slotValueChanged(int);

private:

    RIntNumInputPriv* d;
};

// ---------------------------------------------------------

class RDoubleNumInputPriv;

class LIBKDCRAW_EXPORT RDoubleNumInput : public QHBox
{

Q_OBJECT

public:

    RDoubleNumInput(QWidget *parent=0);
    ~RDoubleNumInput();

    double value();

    void   setPrecision(int p);
    void   setRange(double min, double max, double step, bool slider=true);

    void   setDefaultValue(double d);
    double defaultValue();

    KDoubleNumInput* input() const;

signals:

    void reset();
    void valueChanged(double);

public slots:

    void setValue(double d);
    void slotReset();

private slots:

    void slotValueChanged(double);

private:

    RDoubleNumInputPriv* d;
};

}  // namespace KDcrawIface

#endif /* RNUMINPUT_H */
