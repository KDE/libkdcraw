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

// Qt includes.

#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qlabel.h>

// KDE includes.

#include <klocale.h>
#include <kiconloader.h>

// Local includes.

#include "rnuminput.h"
#include "rnuminput.moc"

namespace KDcrawIface
{

class RIntNumInputPriv
{

public:

    RIntNumInputPriv()
    {
        defaultValue = 0;
        resetButton  = 0;
        input        = 0;
    }

    int           defaultValue;

    QToolButton  *resetButton;

    KIntNumInput *input;
};

RIntNumInput::RIntNumInput(QWidget *parent)
            : QHBox(parent)
{
    d = new RIntNumInputPriv;

    d->input       = new KIntNumInput(this);
    QLabel *space  = new QLabel(this);
    d->resetButton = new QToolButton(this);
    d->resetButton->setAutoRaise(true);
    d->resetButton->setFocusPolicy(QWidget::NoFocus);
    d->resetButton->setIconSet(SmallIconSet("reload_page"));
    QToolTip::add(d->resetButton, i18n("Reset to default value"));

    setStretchFactor(space, 10);
    setMargin(0);
    setSpacing(0);

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

void RIntNumInput::setRange(int min, int max, int step)
{
    d->input->setRange(min, max, step);
}

int RIntNumInput::value()
{
    return d->input->value();
}

void RIntNumInput::setValue(int v)
{
    d->input->setValue(v);
}

int RIntNumInput::defaultValue()
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
    if (v != d->defaultValue)
        d->resetButton->setEnabled(true);

    emit valueChanged(v);
}

// ----------------------------------------------------

class RDoubleNumInputPriv
{

public:

    RDoubleNumInputPriv()
    {
        defaultValue = 0.0;
        resetButton  = 0;
        input        = 0;
    }

    double           defaultValue;

    QToolButton     *resetButton;

    KDoubleNumInput *input;
};

RDoubleNumInput::RDoubleNumInput(QWidget *parent)
               : QHBox(parent)
{
    d = new RDoubleNumInputPriv;

    d->input       = new KDoubleNumInput(this);
    QLabel *space  = new QLabel(this);
    d->resetButton = new QToolButton(this);
    d->resetButton->setAutoRaise(true);
    d->resetButton->setFocusPolicy(QWidget::NoFocus);
    d->resetButton->setIconSet(SmallIconSet("reload_page"));
    QToolTip::add(d->resetButton, i18n("Reset to default value"));

    setStretchFactor(space, 10);
    setMargin(0);
    setSpacing(0);

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

void RDoubleNumInput::setPrecision(int p)
{
    d->input->setPrecision(p);
}

void RDoubleNumInput::setRange(int min, int max, int step)
{
    d->input->setRange(min, max, step);
}

double RDoubleNumInput::value()
{
    return d->input->value();
}

void RDoubleNumInput::setValue(double v)
{
    d->input->setValue(v);
}

double RDoubleNumInput::defaultValue()
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
    if (v != d->defaultValue)
        d->resetButton->setEnabled(true);

    emit valueChanged(v);
}

}  // namespace KDcrawIface
