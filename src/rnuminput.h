/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2008-08-16
 * @brief  Integer and double num input widget
 *         re-implemented with a reset button to switch to
 *         a default value
 *
 * @author Copyright (C) 2008-2013 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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

#ifndef RNUMINPUT_H
#define RNUMINPUT_H

// KDE includes

#include <khbox.h>
#include <knuminput.h>

// Local includes

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class LIBKDCRAW_EXPORT RIntNumInput : public KHBox
{
    Q_OBJECT

public:

    RIntNumInput(QWidget* const parent=0);
    ~RIntNumInput();

    int  value() const;

    void setSliderEnabled(bool b);
    void setRange(int min, int max, int step);

    void setDefaultValue(int d);
    int  defaultValue() const;

    KIntNumInput* input() const;

Q_SIGNALS:

    void reset();
    void valueChanged(int);

public Q_SLOTS:

    void setValue(int d);
    void slotReset();

private Q_SLOTS:

    void slotValueChanged(int);

private:

    class Private;
    Private* const d;
};

// ---------------------------------------------------------

class LIBKDCRAW_EXPORT RDoubleNumInput : public KHBox
{
    Q_OBJECT

public:

    RDoubleNumInput(QWidget* const parent=0);
    ~RDoubleNumInput();

    double value() const;

    void   setDecimals(int p);
    void   setRange(double min, double max, double step, bool slider=true);

    void   setDefaultValue(double d);
    double defaultValue() const;

    KDoubleNumInput* input() const;

Q_SIGNALS:

    void reset();
    void valueChanged(double);

public Q_SLOTS:

    void setValue(double d);
    void slotReset();

private Q_SLOTS:

    void slotValueChanged(double);

private:

    class Private;
    Private* const d;
};

}  // namespace KDcrawIface

#endif /* RNUMINPUT_H */
