/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-08-16
 * Description : a combo box widget re-implemented with a
 *               reset button to switch to a default item.
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

#ifndef RCOMBOBOX_H
#define RCOMBOBOX_H

// Qt includes.

#include <qhbox.h>

// KDE includes.

#include <kcombobox.h>

// Local includes.

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class RComboBoxPriv;

class LIBKDCRAW_EXPORT RComboBox : public QHBox
{

Q_OBJECT

public:

    RComboBox(QWidget *parent=0);
    ~RComboBox();

    void setCurrentItem(int d);
    int  currentItem();

    void setDefaultItem(int d);
    int  defaultItem();

    KComboBox* combo() const;

    void insertItem(const QString& t, int index=-1);

signals:

    void reset();
    void activated(int);
    void currentIndexChanged(int);

public slots:

    void slotReset();

private slots:

    void slotItemActivated(int);

private:

    RComboBoxPriv* d;
};

}  // namespace KDcrawIface

#endif /* RCOMBOBOX_H */
