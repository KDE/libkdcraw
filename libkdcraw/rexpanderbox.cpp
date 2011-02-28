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

#include "rexpanderbox.moc"

// Qt includes

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QCursor>
#include <QStyle>
#include <QStyleOption>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QCheckBox>

// KDE includes

#include <kseparator.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kdialog.h>
#include <klocale.h>
#include <kconfig.h>

namespace KDcrawIface
{

RClickLabel::RClickLabel(QWidget* parent)
           : QLabel(parent)
{
    setCursor(Qt::PointingHandCursor);
}

RClickLabel::RClickLabel(const QString& text, QWidget* parent)
           : QLabel(text, parent)
{
    setCursor(Qt::PointingHandCursor);
}

void RClickLabel::mousePressEvent(QMouseEvent* event)
{
    QLabel::mousePressEvent(event);

    /*
     * In some contexts, like QGraphicsView, there will be no
     * release event if the press event was not accepted.
     */
    if (event->button() == Qt::LeftButton)
    {
        event->accept();
    }
}

void RClickLabel::mouseReleaseEvent(QMouseEvent* event)
{
    QLabel::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        emit leftClicked();
        emit activated();
        event->accept();
    }
}

void RClickLabel::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_Down:
        case Qt::Key_Right:
        case Qt::Key_Space:
            emit activated();
            return;
        default:
            break;
    }

    QLabel::keyPressEvent(e);
}

// ------------------------------------------------------------------------

RSqueezedClickLabel::RSqueezedClickLabel(QWidget* parent)
                   : KSqueezedTextLabel(parent)
{
    setCursor(Qt::PointingHandCursor);
}

RSqueezedClickLabel::RSqueezedClickLabel(const QString& text, QWidget* parent)
                   : KSqueezedTextLabel(text, parent)
{
    setCursor(Qt::PointingHandCursor);
}

void RSqueezedClickLabel::mouseReleaseEvent(QMouseEvent* event)
{
    KSqueezedTextLabel::mouseReleaseEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        emit leftClicked();
        emit activated();
        event->accept();
    }
}

void RSqueezedClickLabel::mousePressEvent(QMouseEvent* event)
{
    QLabel::mousePressEvent(event);

    /*
     * In some contexts, like QGraphicsView, there will be no
     * release event if the press event was not accepted.
     */
    if (event->button() == Qt::LeftButton)
    {
        event->accept();
    }
}

void RSqueezedClickLabel::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_Down:
        case Qt::Key_Right:
        case Qt::Key_Space:
            emit activated();
            return;
        default:
            break;
    }

    QLabel::keyPressEvent(e);
}

// ------------------------------------------------------------------------

RArrowClickLabel::RArrowClickLabel(QWidget* parent)
                : QWidget(parent), m_arrowType(Qt::DownArrow)
{
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_size   = 8;
    m_margin = 2;
}

void RArrowClickLabel::setArrowType(Qt::ArrowType type)
{
    m_arrowType = type;
    update();
}

Qt::ArrowType RArrowClickLabel::arrowType() const
{
    return m_arrowType;
}

void RArrowClickLabel::mousePressEvent(QMouseEvent* event)
{
    /*
     * In some contexts, like QGraphicsView, there will be no
     * release event if the press event was not accepted.
     */
    if (event->button() == Qt::LeftButton)
    {
        event->accept();
    }
}

void RArrowClickLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit leftClicked();
    }
}

void RArrowClickLabel::paintEvent(QPaintEvent*)
{
    // Inspired by karrowbutton.cpp,
    //  Copyright (C) 2001 Frerich Raabe <raabe@kde.org>

    QPainter p(this);

    QStyleOptionFrame opt;
    opt.init(this);
    opt.lineWidth    = 2;
    opt.midLineWidth = 0;

    /*
    p.fillRect( rect(), palette().brush( QPalette::Background ) );
    style()->drawPrimitive( QStyle::PE_Frame, &opt, &p, this);
    */

    if (m_arrowType == Qt::NoArrow)
        return;

    if (width() < m_size + m_margin ||
        height() < m_size + m_margin)
        return; // don't draw arrows if we are too small

    unsigned int x = 0, y = 0;
    if (m_arrowType == Qt::DownArrow)
    {
        x = (width() - m_size) / 2;
        y = height() - (m_size + m_margin);
    }
    else if (m_arrowType == Qt::UpArrow)
    {
        x = (width() - m_size) / 2;
        y = m_margin;
    }
    else if (m_arrowType == Qt::RightArrow)
    {
        x = width() - (m_size + m_margin);
        y = (height() - m_size) / 2;
    }
    else // arrowType == LeftArrow
    {
        x = m_margin;
        y = (height() - m_size) / 2;
    }

    /*
    if (isDown())
    {
        ++x;
        ++y;
    }
    */

    QStyle::PrimitiveElement e = QStyle::PE_IndicatorArrowLeft;
    switch (m_arrowType)
    {
        case Qt::LeftArrow:
            e = QStyle::PE_IndicatorArrowLeft;
            break;
        case Qt::RightArrow:
            e = QStyle::PE_IndicatorArrowRight;
            break;
        case Qt::UpArrow:
            e = QStyle::PE_IndicatorArrowUp;
            break;
        case Qt::DownArrow:
            e = QStyle::PE_IndicatorArrowDown;
            break;
        case Qt::NoArrow:
            break;
    }

    opt.state |= QStyle::State_Enabled;
    opt.rect   = QRect( x, y, m_size, m_size);

    style()->drawPrimitive( e, &opt, &p, this );
}

QSize RArrowClickLabel::sizeHint() const
{
    return QSize(m_size + 2*m_margin, m_size + 2*m_margin);
}

// ------------------------------------------------------------------------

class RLabelExpander::RLabelExpanderPriv
{

public:

    RLabelExpanderPriv()
    {
        clickLabel      = 0;
        containerWidget = 0;
        pixmapLabel     = 0;
        grid            = 0;
        arrow           = 0;
        line            = 0;
        hbox            = 0;
        checkBox        = 0;
        expandByDefault = true;
    }

    bool              expandByDefault;

    QCheckBox*        checkBox;
    QLabel*           pixmapLabel;
    QWidget*          containerWidget;
    QGridLayout*      grid;

    KSeparator*       line;
    QWidget*          hbox;

    RArrowClickLabel* arrow;
    RClickLabel*      clickLabel;
};

RLabelExpander::RLabelExpander(QWidget* parent)
              : QWidget(parent), d(new RLabelExpanderPriv)
{
    d->grid        = new QGridLayout(this);
    d->line        = new KSeparator(Qt::Horizontal, this);
    d->hbox        = new QWidget(this);
    d->arrow       = new RArrowClickLabel(d->hbox);
    d->checkBox    = new QCheckBox(d->hbox);
    d->pixmapLabel = new QLabel(d->hbox);
    d->clickLabel  = new RClickLabel(d->hbox);

    QHBoxLayout* hlay = new QHBoxLayout(d->hbox);
    hlay->addWidget(d->arrow);
    hlay->addWidget(d->checkBox);
    hlay->addWidget(d->pixmapLabel);
    hlay->addWidget(d->clickLabel, 10);
    hlay->setMargin(0);
    hlay->setSpacing(KDialog::spacingHint());

    d->pixmapLabel->installEventFilter(this);
    d->pixmapLabel->setCursor(Qt::PointingHandCursor);

    d->hbox->setCursor(Qt::PointingHandCursor);
    setCheckBoxVisible(false);

    d->grid->addWidget(d->line, 0, 0, 1, 3);
    d->grid->addWidget(d->hbox, 1, 0, 1, 3);
    d->grid->setColumnStretch(2, 10);
    d->grid->setMargin(KDialog::spacingHint());
    d->grid->setSpacing(KDialog::spacingHint());

    connect(d->arrow, SIGNAL(leftClicked()),
            this, SLOT(slotToggleContainer()));

    connect(d->clickLabel, SIGNAL(activated()),
            this, SLOT(slotToggleContainer()));

    connect(d->checkBox, SIGNAL(toggled(bool)),
            this, SIGNAL(signalToggled(bool)));
}

RLabelExpander::~RLabelExpander()
{
    delete d;
}

void RLabelExpander::setCheckBoxVisible(bool b)
{
    d->checkBox->setVisible(b);
}

bool RLabelExpander::checkBoxIsVisible() const
{
    return d->checkBox->isVisible();
}

void RLabelExpander::setChecked(bool b)
{
    d->checkBox->setChecked(b);
}

bool RLabelExpander::isChecked() const
{
    return d->checkBox->isChecked();
}

void RLabelExpander::setLineVisible(bool b)
{
    d->line->setVisible(b);
}

bool RLabelExpander::lineIsVisible() const
{
    return d->line->isVisible();
}

void RLabelExpander::setText(const QString& txt)
{
    d->clickLabel->setText(QString("<qt><b>%1</b></qt>").arg(txt));
}

QString RLabelExpander::text() const
{
    return d->clickLabel->text();
}

void RLabelExpander::setIcon(const QPixmap& pix)
{
    d->pixmapLabel->setPixmap(pix);
}

const QPixmap* RLabelExpander::icon() const
{
    return d->pixmapLabel->pixmap();
}

void RLabelExpander::setWidget(QWidget* widget)
{
    if (widget)
    {
        d->containerWidget = widget;
        d->containerWidget->setParent(this);
        d->grid->addWidget(d->containerWidget, 2, 0, 1, 3);
    }
}

QWidget* RLabelExpander::widget() const
{
    return d->containerWidget;
}

void RLabelExpander::setExpandByDefault(bool b)
{
    d->expandByDefault = b;
}

bool RLabelExpander::isExpandByDefault() const
{
    return d->expandByDefault;
}

void RLabelExpander::setExpanded(bool b)
{
    if (d->containerWidget)
    {
        d->containerWidget->setVisible(b);
        if (b)
           d->arrow->setArrowType(Qt::DownArrow);
        else
           d->arrow->setArrowType(Qt::RightArrow);
    }

    emit signalExpanded(b);
}

bool RLabelExpander::isExpanded() const
{
    return (d->arrow->arrowType() == Qt::DownArrow);
}

void RLabelExpander::slotToggleContainer()
{
    if (d->containerWidget)
        setExpanded(!d->containerWidget->isVisible());
}

bool RLabelExpander::eventFilter(QObject* obj, QEvent* ev)
{
    if ( obj == d->pixmapLabel)
    {
        if ( ev->type() == QEvent::MouseButtonRelease)
        {
            slotToggleContainer();
            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // pass the event on to the parent class
        return QWidget::eventFilter(obj, ev);
    }
}

// ------------------------------------------------------------------------

class RExpanderBox::RExpanderBoxPriv
{

public:

    RExpanderBoxPriv(RExpanderBox* box)
    {
        parent = box;
        vbox   = 0;
    }

    void createItem(int index, QWidget* w, const QPixmap& pix, const QString& txt,
                    const QString& objName, bool expandBydefault)
    {
        RLabelExpander* exp = new RLabelExpander(parent->viewport());
        exp->setText(txt);
        exp->setIcon(pix);
        exp->setWidget(w);
        exp->setLineVisible(!wList.isEmpty());
        exp->setObjectName(objName);
        exp->setExpandByDefault(expandBydefault);

        if (index >= 0)
        {
            vbox->insertWidget(index, exp);
            wList.insert(index, exp);
        }
        else
        {
            vbox->addWidget(exp);
            wList.append(exp);
        }

        parent->connect(exp, SIGNAL(signalExpanded(bool)),
                        parent, SLOT(slotItemExpanded(bool)));

        parent->connect(exp, SIGNAL(signalToggled(bool)),
                        parent, SLOT(slotItemToggled(bool)));
    }

    QList<RLabelExpander*> wList;

    QVBoxLayout*           vbox;

    RExpanderBox*          parent;
};

RExpanderBox::RExpanderBox(QWidget* parent)
            : QScrollArea(parent), d(new RExpanderBoxPriv(this))
{
    setFrameStyle(QFrame::NoFrame);
    setWidgetResizable(true);
    QWidget* main = new QWidget(viewport());
    d->vbox       = new QVBoxLayout(main);
    d->vbox->setMargin(0);
    d->vbox->setSpacing(KDialog::spacingHint());
    setWidget(main);

    setAutoFillBackground(false);
    viewport()->setAutoFillBackground(false);
    main->setAutoFillBackground(false);
}

RExpanderBox::~RExpanderBox()
{
    writeSettings();
    d->wList.clear();
    delete d;
}

void RExpanderBox::setCheckBoxVisible(int index, bool b)
{
    if (index > d->wList.count() || index < 0) return;
    d->wList[index]->setCheckBoxVisible(b);
}

bool RExpanderBox::checkBoxIsVisible(int index) const
{
    if (index > d->wList.count() || index < 0) return false;
    return d->wList[index]->checkBoxIsVisible();
}

void RExpanderBox::setChecked(int index, bool b)
{
    if (index > d->wList.count() || index < 0) return;
    d->wList[index]->setChecked(b);
}

bool RExpanderBox::isChecked(int index) const
{
    if (index > d->wList.count() || index < 0) return false;
    return d->wList[index]->isChecked();
}

void RExpanderBox::addItem(QWidget* w, const QPixmap& pix, const QString& txt,
                           const QString& objName, bool expandBydefault)
{
    d->createItem(-1, w, pix, txt, objName, expandBydefault);
}

void RExpanderBox::addItem(QWidget* w, const QString& txt,
                           const QString& objName, bool expandBydefault)
{
    addItem(w, QPixmap(), txt, objName, expandBydefault);
}

void RExpanderBox::addStretch()
{
    d->vbox->addStretch(10);
}

void RExpanderBox::insertItem(int index, QWidget* w, const QPixmap& pix, const QString& txt,
                              const QString& objName, bool expandBydefault)
{
    d->createItem(index, w, pix, txt, objName, expandBydefault);
}

void RExpanderBox::slotItemExpanded(bool b)
{
    RLabelExpander* exp = dynamic_cast<RLabelExpander*>(sender());
    if (exp)
    {
        int index = indexOf(exp);
        emit signalItemExpanded(index, b);
    }
}

void RExpanderBox::slotItemToggled(bool b)
{
    RLabelExpander* exp = dynamic_cast<RLabelExpander*>(sender());
    if (exp)
    {
        int index = indexOf(exp);
        emit signalItemToggled(index, b);
    }
}

void RExpanderBox::insertItem(int index, QWidget* w, const QString& txt,
                              const QString& objName, bool expandBydefault)
{
    insertItem(index, w, QPixmap(), txt, objName, expandBydefault);
}

void RExpanderBox::insertStretch(int index)
{
    d->vbox->insertStretch(index, 10);
}

void RExpanderBox::removeItem(int index)
{
    if (index > d->wList.count() || index < 0) return;
    d->wList[index]->hide();
    d->wList.removeAt(index);
}

void RExpanderBox::setItemText(int index, const QString& txt)
{
    if (index > d->wList.count() || index < 0) return;
    d->wList[index]->setText(txt);
}

QString RExpanderBox::itemText(int index) const
{
    if (index > d->wList.count() || index < 0) return QString();
    return d->wList[index]->text();
}

void RExpanderBox::setItemIcon(int index, const QPixmap& pix)
{
    if (index > d->wList.count() || index < 0) return;
    d->wList[index]->setIcon(pix);
}

const QPixmap* RExpanderBox::itemIcon(int index) const
{
    if (index > d->wList.count() || index < 0) return 0;
    return d->wList[index]->icon();
}

int RExpanderBox::count() const
{
    return d->wList.count();
}

void RExpanderBox::setItemToolTip(int index, const QString& tip)
{
    if (index > d->wList.count() || index < 0) return;
    d->wList[index]->setToolTip(tip);
}

QString RExpanderBox::itemToolTip(int index) const
{
    if (index > d->wList.count() || index < 0) return QString();
    return d->wList[index]->toolTip();
}

void RExpanderBox::setItemEnabled(int index, bool enabled)
{
    if (index > d->wList.count() || index < 0) return;
    d->wList[index]->setEnabled(enabled);
}

bool RExpanderBox::isItemEnabled(int index) const
{
    if (index > d->wList.count() || index < 0) return false;
    return d->wList[index]->isEnabled();
}

RLabelExpander* RExpanderBox::widget(int index) const
{
    if (index > d->wList.count() || index < 0) return 0;

    return d->wList[index];
}

int RExpanderBox::indexOf(RLabelExpander* widget) const
{
    for (int i = 0 ; i < count(); ++i)
    {
        RLabelExpander *exp = d->wList[i];
        if (widget == exp)
            return i;
    }
    return -1;
}

void RExpanderBox::setItemExpanded(int index, bool b)
{
    if (index > d->wList.count() || index < 0) return;

    RLabelExpander *exp = d->wList[index];
    if (!exp) return;

    exp->setExpanded(b);
}

bool RExpanderBox::isItemExpanded(int index) const
{
    if (index > d->wList.count() || index < 0) return false;

    RLabelExpander *exp = d->wList[index];
    if (!exp) return false;

    return (exp->isExpanded());
}

void RExpanderBox::readSettings()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(QString("%1").arg(objectName()));

    for (int i = 0 ; i < count(); ++i)
    {
        RLabelExpander *exp = d->wList[i];
        if (exp)
        {
            exp->setExpanded(group.readEntry(QString("%1 Expanded").arg(exp->objectName()),
                                             exp->isExpandByDefault()));
        }
    }
}

void RExpanderBox::writeSettings()
{
    KSharedConfig::Ptr config = KGlobal::config();
    KConfigGroup group        = config->group(QString("%1").arg(objectName()));

    for (int i = 0 ; i < count(); ++i)
    {
        RLabelExpander *exp = d->wList[i];
        if (exp)
        {
            group.writeEntry(QString("%1 Expanded").arg(exp->objectName()),
                             exp->isExpanded());
        }
    }
    config->sync();
}

// ------------------------------------------------------------------------

RExpanderBoxExclusive::RExpanderBoxExclusive(QWidget* parent)
                     : RExpanderBox(parent)
{
    setIsToolBox(true);
}

RExpanderBoxExclusive::~RExpanderBoxExclusive()
{
}

void RExpanderBoxExclusive::slotItemExpanded(bool b)
{
    RLabelExpander* exp = dynamic_cast<RLabelExpander*>(sender());
    if (!exp) return;

    if (isToolBox() && b)
    {
        int item = 0;

        while (item < count())
        {
            if (isItemExpanded(item) && item != indexOf(exp))
            {
                setItemExpanded(item, false);
            }
            item++;
        }
    }
    emit signalItemExpanded(indexOf(exp), b);
}

void RExpanderBoxExclusive::setIsToolBox(bool b)
{
    m_toolbox = b;
}

bool RExpanderBoxExclusive::isToolBox() const
{
    return (m_toolbox);
}

} // namespace KDcrawIface
