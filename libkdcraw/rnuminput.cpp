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

#include "rnuminput.moc"

// Qt includes

#include <QToolButton>

// KDE includes

#include <klocale.h>
#include <kiconloader.h>
#include <kdialog.h>

namespace KDcrawIface
{

class RIntNumInput::Private
{

public:

    Private()
    {
        defaultValue = 0;
        resetButton  = 0;
        input        = 0;
    }

    int           defaultValue;

    QToolButton*  resetButton;

    KIntNumInput* input;
};

RIntNumInput::RIntNumInput(QWidget* const parent)
    : KHBox(parent), d(new Private)
{
    d->input       = new KIntNumInput(this);
    d->resetButton = new QToolButton(this);
    d->resetButton->setAutoRaise(true);
    d->resetButton->setFocusPolicy(Qt::NoFocus);
    d->resetButton->setIcon(SmallIcon("document-revert"));
    d->resetButton->setToolTip(i18nc("@info:tooltip", "Reset to default value"));

    setStretchFactor(d->input, 10);
    setMargin(0);
    setSpacing(KDialog::spacingHint());

    // -------------------------------------------------------------

    connect(d->resetButton, SIGNAL(clicked()),
            this, SLOT(slotReset()));

    connect(d->input, SIGNAL(valueChanged(int)),
            this, SLOT(slotValueChanged(int)));
}

RIntNumInput::~RIntNumInput()
{
    delete d;
}

KIntNumInput* RIntNumInput::input() const
{
    return d->input;
}

void RIntNumInput::setSliderEnabled(bool b)
{
    d->input->setSliderEnabled(b);
}

void RIntNumInput::setRange(int min, int max, int step)
{
    d->input->setRange(min, max, step);
}

int RIntNumInput::value() const
{
    return d->input->value();
}

void RIntNumInput::setValue(int v)
{
    d->input->setValue(v);
}

int RIntNumInput::defaultValue() const
{
    return d->defaultValue;
}

void RIntNumInput::setDefaultValue(int v)
{
    d->defaultValue = v;
    d->input->setValue(d->defaultValue);
    slotValueChanged(v);
}

void RIntNumInput::slotReset()
{
    d->input->setValue(d->defaultValue);
    d->resetButton->setEnabled(false);
    emit reset();
}

void RIntNumInput::slotValueChanged(int v)
{
    d->resetButton->setEnabled(v != d->defaultValue);
    emit valueChanged(v);
}

// ----------------------------------------------------

class RDoubleNumInput::Private
{

public:

    Private()
    {
        defaultValue = 0.0;
        resetButton  = 0;
        input        = 0;
    }

    double           defaultValue;

    QToolButton*     resetButton;

    KDoubleNumInput* input;
};

RDoubleNumInput::RDoubleNumInput(QWidget* const parent)
    : KHBox(parent), d(new Private)
{
    d->input       = new KDoubleNumInput(this);
    d->resetButton = new QToolButton(this);
    d->resetButton->setAutoRaise(true);
    d->resetButton->setFocusPolicy(Qt::NoFocus);
    d->resetButton->setIcon(SmallIcon("document-revert"));
    d->resetButton->setToolTip(i18nc("@info:tooltip", "Reset to default value"));

    setStretchFactor(d->input, 10);
    setMargin(0);
    setSpacing(KDialog::spacingHint());

    // -------------------------------------------------------------

    connect(d->resetButton, SIGNAL(clicked()),
            this, SLOT(slotReset()));

    connect(d->input, SIGNAL(valueChanged(double)),
            this, SLOT(slotValueChanged(double)));
}

RDoubleNumInput::~RDoubleNumInput()
{
    delete d;
}

KDoubleNumInput* RDoubleNumInput::input() const
{
    return d->input;
}

void RDoubleNumInput::setDecimals(int p)
{
    d->input->setDecimals(p);
}

void RDoubleNumInput::setRange(double min, double max, double step, bool slider)
{
    d->input->setRange(min, max, step, slider);
}

double RDoubleNumInput::value() const
{
    return d->input->value();
}

void RDoubleNumInput::setValue(double v)
{
    d->input->setValue(v);
}

double RDoubleNumInput::defaultValue() const
{
    return d->defaultValue;
}

void RDoubleNumInput::setDefaultValue(double v)
{
    d->defaultValue = v;
    d->input->setValue(d->defaultValue);
    slotValueChanged(v);
}

void RDoubleNumInput::slotReset()
{
    d->input->setValue(d->defaultValue);
    d->resetButton->setEnabled(false);
    emit reset();
}

void RDoubleNumInput::slotValueChanged(double v)
{
    d->resetButton->setEnabled(v != d->defaultValue);
    emit valueChanged(v);
}

}  // namespace KDcrawIface
