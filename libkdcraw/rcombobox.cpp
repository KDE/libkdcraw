/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-08-16
 * Description : a combo box widget re-implemented with a
 *               reset button to switch to a default item.
 *
 * Copyright (C) 2008-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "rcombobox.h"
#include "rcombobox.moc"

// Qt includes.

#include <QToolButton>

// KDE includes.

#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>

namespace KDcrawIface
{

class RComboBoxPriv
{

public:

    RComboBoxPriv()
    {
        defaultIndex = 0;
        resetButton  = 0;
        combo        = 0;
    }

    int          defaultIndex;

    QToolButton *resetButton;

    KComboBox   *combo;
};

RComboBox::RComboBox(QWidget *parent)
         : KHBox(parent), d(new RComboBoxPriv)
{
    d->combo       = new KComboBox(this);
    d->resetButton = new QToolButton(this);
    d->resetButton->setAutoRaise(true);
    d->resetButton->setFocusPolicy(Qt::NoFocus);
    d->resetButton->setIcon(SmallIcon("document-revert"));
    d->resetButton->setToolTip(i18n("Reset to default value"));

    setStretchFactor(d->combo, 10);
    setMargin(0);
    setSpacing(KDialog::spacingHint());

    // -------------------------------------------------------------

    connect(d->resetButton, SIGNAL(clicked()),
            this, SLOT(slotReset()));

    connect(d->combo, SIGNAL(activated(int)),
            this, SLOT(slotItemActivated(int)));

    connect(d->combo, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(currentIndexChanged(int)));
}

RComboBox::~RComboBox()
{
    delete d;
}

KComboBox* RComboBox::combo() const
{
    return d->combo;
}

void RComboBox::addItem(const QString& t, int index)
{
    d->combo->addItem(t, index);
}

void RComboBox::insertItem(int index, const QString& t)
{
    d->combo->insertItem(index, t);
}

int RComboBox::currentIndex()
{
    return d->combo->currentIndex();
}

void RComboBox::setCurrentIndex(int v)
{
    d->combo->setCurrentIndex(v);
}

int RComboBox::defaultIndex()
{
    return d->defaultIndex;
}

void RComboBox::setDefaultIndex(int v)
{
    d->defaultIndex = v;
    d->combo->setCurrentIndex(d->defaultIndex);
    slotItemActivated(v);
}

void RComboBox::slotReset()
{
    d->combo->setCurrentIndex(d->defaultIndex);
    d->resetButton->setEnabled(false);
    slotItemActivated(d->defaultIndex);
    emit reset();
}

void RComboBox::slotItemActivated(int v)
{
    if (v != d->defaultIndex)
        d->resetButton->setEnabled(true);

    emit activated(v);
}

}  // namespace KDcrawIface
