/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2014-09-12
 * @brief  Simple widgets collection
 *
 * @author Copyright (C) 2014-2015 by Gilles Caulier
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

#ifndef RWIDGETUTILS_h
#define RWIDGETUTILS_h

// Qt includes

#include <QLabel>
#include <QUrl>
#include <QString>
#include <QFrame>

// Local includes

#include "libkdcraw_export.h"

namespace KDcrawIface
{

/** A widget to host an image into a label with an active url which can be 
 *  open to default web browser using simple mouse click.
 */
class LIBKDCRAW_EXPORT RActiveLabel : public QLabel
{
    Q_OBJECT

public:

    explicit RActiveLabel(const QUrl& url=QUrl(), const QString& imgPath=QString(), QWidget* const parent=0);
    virtual ~RActiveLabel();
    
    void updateData(const QUrl& url, const QImage& img);
};

// ------------------------------------------------------------------------------------

/**
 * A widget to show an horizontal or vertical line separator 
 **/
class LIBKDCRAW_EXPORT RLineWidget : public QFrame
{
    Q_OBJECT

public:

    explicit RLineWidget(Qt::Orientation orientation, QWidget* const parent=0);
    virtual ~RLineWidget();
};

// ------------------------------------------------------------------------------------

/** An Horizontal widget to host children widgets
 */
class LIBKDCRAW_EXPORT RHBox : public QFrame
{
    Q_OBJECT
    Q_DISABLE_COPY(RHBox)

public:

    explicit RHBox(QWidget* const parent=0);
    virtual ~RHBox();

    void setMargin(int margin);
    void setSpacing(int space);
    void setStretchFactor(QWidget* const widget, int stretch);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

protected:

    RHBox(bool vertical, QWidget* const parent);

    virtual void childEvent(QChildEvent* e);
};

// ------------------------------------------------------------------------------------

/** A Vertical widget to host children widgets
 */
class LIBKDCRAW_EXPORT RVBox : public RHBox
{
    Q_OBJECT
    Q_DISABLE_COPY(RVBox)

  public:

    explicit RVBox(QWidget* const parent=0);
    virtual ~RVBox();
};

} // namespace KDcrawIface

#endif // RWIDGETUTILS_h
