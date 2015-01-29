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

#include "rwidgetutils.h"

// Qt includes

#include <QWidget>
#include <QByteArray>
#include <QBuffer>
#include <QImage>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QApplication>

namespace KDcrawIface
{

RActiveLabel::RActiveLabel(const QUrl& url, const QString& imgPath, QWidget* const parent)
    : QLabel(parent)
{
    setMargin(0);
    setScaledContents(false);
    setOpenExternalLinks(true);
    setTextFormat(Qt::RichText);
    setFocusPolicy(Qt::NoFocus);
    setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
    QImage img = QImage(imgPath);

    updateData(url, img);
}

RActiveLabel::~RActiveLabel()
{
}

void RActiveLabel::updateData(const QUrl& url, const QImage& img)
{
    QByteArray byteArray;
    QBuffer    buffer(&byteArray);
    img.save(&buffer, "PNG");
    setText(QString::fromLatin1("<a href=\"%1\">%2</a>")
            .arg(url.url())
            .arg(QString::fromLatin1("<img src=\"data:image/png;base64,%1\">")
            .arg(QString::fromLatin1(byteArray.toBase64().data()))));
}

// ------------------------------------------------------------------------------------

RLineWidget::RLineWidget(Qt::Orientation orientation, QWidget* const parent)
    : QFrame(parent)
{
    setLineWidth(1);
    setMidLineWidth(0);
    
    if (orientation == Qt::Vertical)
    {
        setFrameShape(QFrame::VLine);
        setFrameShadow(QFrame::Sunken);
        setMinimumSize(2, 0);
    }
    else
    {
        setFrameShape(QFrame::HLine);
        setFrameShadow(QFrame::Sunken);
        setMinimumSize(0, 2);
    }
   
    updateGeometry();
}

RLineWidget::~RLineWidget()
{
}

// ------------------------------------------------------------------------------------

RHBox::RHBox(QWidget* const parent)
    : QFrame(parent)
{
    QHBoxLayout* const layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
}


RHBox::RHBox(bool /*vertical*/, QWidget* const parent)
    : QFrame(parent)
{
    QVBoxLayout* const layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    setLayout(layout);
}

RHBox::~RHBox()
{
}

void RHBox::childEvent(QChildEvent* e)
{
    switch (e->type())
    {
        case QEvent::ChildAdded:
        {
            QChildEvent* const ce = static_cast<QChildEvent*>(e);

            if (ce->child()->isWidgetType())
            {
                QWidget* const w = static_cast<QWidget*>(ce->child());
                static_cast<QBoxLayout*>(layout())->addWidget(w);
            }

            break;
        }

        case QEvent::ChildRemoved:
        {
            QChildEvent* const ce = static_cast<QChildEvent*>(e);

            if (ce->child()->isWidgetType())
            {
                QWidget* const w = static_cast<QWidget*>(ce->child());
                static_cast<QBoxLayout*>(layout())->removeWidget(w);
            }

            break;
        }

        default:
            break;
    }

    QFrame::childEvent(e);
}

QSize RHBox::sizeHint() const
{
    RHBox* const b = const_cast<RHBox*>(this);
    QApplication::sendPostedEvents(b, QEvent::ChildAdded);

    return QFrame::sizeHint();
}

QSize RHBox::minimumSizeHint() const
{
    RHBox* const b = const_cast<RHBox*>(this);
    QApplication::sendPostedEvents(b, QEvent::ChildAdded );

    return QFrame::minimumSizeHint();
}

void RHBox::setSpacing(int spacing)
{
    layout()->setSpacing(spacing);
}

void RHBox::setMargin(int margin)
{
    layout()->setMargin(margin);
}

void RHBox::setStretchFactor(QWidget* const widget, int stretch)
{
    static_cast<QBoxLayout*>(layout())->setStretchFactor(widget, stretch);
}

// ------------------------------------------------------------------------------------

RVBox::RVBox(QWidget* const parent)
  : RHBox(true, parent)
{
}

RVBox::~RVBox()
{
}

} // namespace KDcrawIface
