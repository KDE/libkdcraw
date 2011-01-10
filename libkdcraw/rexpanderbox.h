/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date   2008-03-14
 * @brief  A widget to host settings as expander box
 *
 * @author Copyright (C) 2008-2011 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2008-2011 by Marcel Wiesweg
 *         <a href="mailto:marcel dot wiesweg at gmx dot de">marcel dot wiesweg at gmx dot de</a>
 * @author Copyright (C) 2010 by Manuel Viet
 *         <a href="mailto:contact at 13zenrv dot fr">contact at 13zenrv dot fr</a>
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

#ifndef REXPANDERBOX_H
#define REXPANDERBOX_H

// Qt includes

#include <QtCore/QObject>
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QScrollArea>

// KDE includes

#include <ksqueezedtextlabel.h>

// Local includes

#include "libkdcraw_export.h"

namespace KDcrawIface
{

class LIBKDCRAW_EXPORT RClickLabel : public QLabel
{
    Q_OBJECT

public:

    RClickLabel(QWidget* parent = 0);
    RClickLabel(const QString& text, QWidget* parent = 0);
    ~RClickLabel(){};

Q_SIGNALS:

    /// Emitted when activated by left mouse click
    void leftClicked();
    /// Emitted when activated, by mouse or key press
    void activated();

protected:

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
};

// -------------------------------------------------------------------------

class LIBKDCRAW_EXPORT RSqueezedClickLabel : public KSqueezedTextLabel
{
    Q_OBJECT

public:

    RSqueezedClickLabel(QWidget* parent = 0);
    RSqueezedClickLabel(const QString& text, QWidget* parent = 0);
    ~RSqueezedClickLabel(){};

Q_SIGNALS:

    void leftClicked();
    void activated();

protected:

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent* event);
};

// -------------------------------------------------------------------------

class LIBKDCRAW_EXPORT RArrowClickLabel : public QWidget
{
    Q_OBJECT

public:

    RArrowClickLabel(QWidget* parent = 0);
    ~RArrowClickLabel(){};

    void setArrowType(Qt::ArrowType arrowType);
    Qt::ArrowType arrowType() const;

    virtual QSize sizeHint () const;

Q_SIGNALS:

    void leftClicked();

protected:

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void paintEvent(QPaintEvent* event);

protected:

    Qt::ArrowType m_arrowType;
    int           m_size;
    int           m_margin;
};

// -------------------------------------------------------------------------

class LIBKDCRAW_EXPORT RLabelExpander : public QWidget
{
    Q_OBJECT

public:

    RLabelExpander(QWidget* parent = 0);
    ~RLabelExpander();

    void setCheckBoxVisible(bool b);
    bool checkBoxIsVisible() const;

    void setChecked(bool b);
    bool isChecked() const;

    void setLineVisible(bool b);
    bool lineIsVisible() const;

    void setText(const QString& txt);
    QString text() const;

    void setIcon(const QPixmap& pix);
    const QPixmap* icon() const;

    void setWidget(QWidget* widget);
    QWidget* widget() const;

    void setExpanded(bool b);
    bool isExpanded() const;

    void setExpandByDefault(bool b);
    bool isExpandByDefault() const;

Q_SIGNALS:

    void signalExpanded(bool);
    void signalToggled(bool);

private Q_SLOTS:

    void slotToggleContainer();

private:

    bool eventFilter(QObject* obj, QEvent* ev);

private:

    class RLabelExpanderPriv;
    RLabelExpanderPriv* const d;
};

// -------------------------------------------------------------------------

class LIBKDCRAW_EXPORT RExpanderBox : public QScrollArea
{
    Q_OBJECT

public:

    RExpanderBox(QWidget* parent = 0);
    ~RExpanderBox();

    /** Add RLabelExpander item at end of box layout with these settings :
        'w'               : the widget hosted by RLabelExpander.
        'pix'             : pixmap used as icon to item title.
        'txt'             : text used as item title.
        'objName'         : item object name used to read/save expanded settings to rc file.
        'expandBydefault' : item state by default (expanded or not).
     */
    void addItem(QWidget* w, const QPixmap& pix, const QString& txt,
                 const QString& objName, bool expandBydefault);
    void addItem(QWidget* w, const QString& txt,
                 const QString& objName, bool expandBydefault);

    /** Insert RLabelExpander item at box layout index with these settings :
        'w'               : the widget hosted by RLabelExpander.
        'pix'             : pixmap used as icon to item title.
        'txt'             : text used as item title.
        'objName'         : item object name used to read/save expanded settings to rc file.
        'expandBydefault' : item state by default (expanded or not).
     */
    void insertItem(int index, QWidget* w, const QPixmap& pix, const QString& txt,
                    const QString& objName, bool expandBydefault);
    void insertItem(int index, QWidget* w, const QString& txt,
                    const QString& objName, bool expandBydefault);

    void removeItem(int index);

    void setCheckBoxVisible(int index, bool b);
    bool checkBoxIsVisible(int index) const;

    void setChecked(int index, bool b);
    bool isChecked(int index) const;

    void setItemText(int index, const QString& txt);
    QString itemText (int index) const;

    void setItemIcon(int index, const QPixmap& pix);
    const QPixmap* itemIcon(int index) const;

    void setItemToolTip(int index, const QString& tip);
    QString itemToolTip(int index) const;

    void setItemEnabled(int index, bool enabled);
    bool isItemEnabled(int index) const;

    void addStretch();
    void insertStretch(int index);

    void setItemExpanded(int index, bool b);
    bool isItemExpanded(int index) const;

    int  count() const;

    RLabelExpander* widget(int index) const;
    int indexOf(RLabelExpander* widget) const;

    void readSettings();
    void writeSettings();

Q_SIGNALS:

    void signalItemExpanded(int index, bool b);
    void signalItemToggled(int index, bool b);

private Q_SLOTS:

    void slotItemExpanded(bool b);
    void slotItemToggled(bool b);

private:

    class RExpanderBoxPriv;
    RExpanderBoxPriv* const d;
};

// -------------------------------------------------------------------------

class LIBKDCRAW_EXPORT RExpanderBoxExclusive : public RExpanderBox
{
    Q_OBJECT

public:

    RExpanderBoxExclusive(QWidget* parent = 0);
    ~RExpanderBoxExclusive();

    /** Show one expander open at most */
    void setIsToolBox(bool b);
    bool isToolBox() const;

private Q_SLOTS:

    void slotItemExpanded(bool b);

private:

    bool m_toolbox;
};

} // namespace KDcrawIface

#endif // REXPANDERBOX_H
