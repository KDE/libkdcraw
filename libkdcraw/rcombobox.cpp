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

// Qt includes.

#include <qtoolbutton.h>
#include <qtooltip.h>

// KDE includes.

#include <kdialog.h>
#include <klocale.h>
#include <kiconloader.h>

// Local includes.

#include "rcombobox.h"
#include "rcombobox.moc"

namespace KDcrawIface
{

class RComboBoxPriv
{

public:

    RComboBoxPriv()
    {
        defaultItem = 0;
        resetButton = 0;
        combo       = 0;
    }

    int          defaultItem;

    QToolButton *resetButton;

    KComboBox   *combo;
};

RComboBox::RComboBox(QWidget *parent)
         : QHBox(parent)
{
    d = new RComboBoxPriv;

    d->combo       = new KComboBox(this);
    d->resetButton = new QToolButton(this);
    d->resetButton->setAutoRaise(true);
    d->resetButton->setFocusPolicy(QWidget::NoFocus);
    d->resetButton->setIconSet(SmallIconSet("reload_page"));
    QToolTip::add(d->resetButton, i18n("Reset to default value"));

    setStretchFactor(d->combo, 10);
    setMargin(0);
    setSpacing(KDialog::spacingHint());

    // -------------------------------------------------------------

    connect(d->resetButton, SIGNAL(clicked()),
            this, SLOT(slotReset()));

    connect(d->combo, SIGNAL(activated(int)),
            this, SLOT(slotItemActivated(int)));
}

RComboBox::~RComboBox()
{
    delete d;
}

KComboBox* RComboBox::combo() const
{
    return d->combo;
}

void RComboBox::insertItem(const QString& t, int index)
{
    d->combo->insertItem(t, index);
}

int RComboBox::currentItem()
{
    return d->combo->currentItem();
}

void RComboBox::setCurrentItem(int v)
{
    d->combo->setCurrentItem(v);
}

int RComboBox::defaultItem()
{
    return d->defaultItem;
}

void RComboBox::setDefaultItem(int v)
{
    d->defaultItem = v;
    d->combo->setCurrentItem(d->defaultItem);
    slotItemActivated(v);
}

void RComboBox::slotReset()
{
    d->combo->setCurrentItem(d->defaultItem);
    d->resetButton->setEnabled(false);
    slotItemActivated(d->defaultItem);
    emit reset();
}

void RComboBox::slotItemActivated(int v)
{
    if (v != d->defaultItem)
        d->resetButton->setEnabled(true);

    emit activated(v);
}

}  // namespace KDcrawIface
