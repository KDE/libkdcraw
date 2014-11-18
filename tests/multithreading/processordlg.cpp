/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  : 2014-10-17
 * @brief : test for implementation of threadWeaver api
 *
 * @author Copyright (C) 2011-2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 * @author Copyright (C) 2014 by Veaceslav Munteanu
 *         <a href="mailto:veaceslav dot munteanu90 at gmail dot com">veaceslav dot munteanu90 at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "processordlg.h"

// Qt includes

#include <QList>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QDialog>
#include <QThreadPool>
#include <QFileInfo>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QDebug>

// KDE includes

#include <klocale.h>

// Local includes

#include "actionthread.h"

class ProcessorDlg::Private
{
public:

    Private()
    {
        count  = 0;
        page   = 0;
        items  = 0;
        vlay   = 0;
        thread = 0;
    }

    int                  count;

    QWidget*             page;
    QLabel*              items;
    QVBoxLayout*         vlay;
    QDialogButtonBox*    buttons;

    QList<QUrl>          list;

    ActionThread*        thread;
};

ProcessorDlg::ProcessorDlg(const QList<QUrl>& list)
    : QDialog(0), d(new Private)
{
    setModal(false);
    setWindowTitle(i18n("Convert RAW files To PNG"));

    d->buttons              = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Close, this);
    d->thread               = new ActionThread(this);
    d->list                 = list;
    d->count                = d->list.count();
    qDebug() << d->list;

    d->page                 = new QWidget(this);
    QVBoxLayout* const vbx  = new QVBoxLayout(this);
    vbx->addWidget(d->page);
    vbx->addWidget(d->buttons);
    QDialog::setLayout(vbx);

    d->vlay                 = new QVBoxLayout(d->page);
    QLabel* const pid       = new QLabel(i18n("PID : %1",  QCoreApplication::applicationPid()),  this);
    QLabel* const core      = new QLabel(i18n("Core : %1", d->thread->maximumNumberOfThreads()), this);
    d->items                = new QLabel(this);
    d->vlay->addWidget(pid);
    d->vlay->addWidget(core);
    d->vlay->addWidget(d->items);

    foreach (const QUrl& url, d->list)
    {
        QProgressBar* const bar = new QProgressBar(this);
        QString file            = url.toLocalFile();
        bar->setMaximum(100);
        bar->setMinimum(0);
        bar->setValue(100);
        bar->setObjectName(file);
        QFileInfo fi(file);
        bar->setFormat(fi.baseName());
        d->vlay->addWidget(bar);
    }

    d->vlay->addStretch();

    QPushButton* const applyBtn  = d->buttons->button(QDialogButtonBox::Apply);
    QPushButton* const cancelBtn = d->buttons->button(QDialogButtonBox::Close);

    connect(applyBtn, SIGNAL(clicked()),
            this, SLOT(slotStart()));

    connect(cancelBtn, SIGNAL(clicked()),
            this, SLOT(slotStop()));

    connect(d->thread, SIGNAL(starting(QUrl)),
            this, SLOT(slotStarting(QUrl)));

    connect(d->thread, SIGNAL(finished(QUrl)),
            this, SLOT(slotFinished(QUrl)));

    connect(d->thread, SIGNAL(failed(QUrl,QString)),
            this, SLOT(slotFailed(QUrl,QString)));

    updateCount();
    resize(500, 400);
}

ProcessorDlg::~ProcessorDlg()
{
    delete d;
}

void ProcessorDlg::updateCount()
{
    d->items->setText(i18n("Files to process : %1", d->count));
}

void ProcessorDlg::slotStart()
{
    if (d->list.isEmpty()) return;

    d->buttons->button(QDialogButtonBox::Apply)->setDisabled(true);

    d->thread->convertRAWtoPNG(d->list);
    d->thread->start();
}

void ProcessorDlg::slotStop()
{
    d->thread->cancel();
    reject();    
}
    
QProgressBar* ProcessorDlg::findProgressBar(const QUrl& url) const
{
    QList<QProgressBar*> bars = findChildren<QProgressBar*>();

    foreach(QProgressBar* const b, bars)
    {
        if (b->objectName() == url.toLocalFile())
        {
            return b;
        }
    }

    qWarning() << "Cannot found relevant progress bar for " << url.toLocalFile();
    return 0;
}

void ProcessorDlg::slotStarting(const QUrl& url)
{
    qDebug() << "Start to process item " << url.toLocalFile();

    QProgressBar* const b = findProgressBar(url);

    if (b)
    {
        b->setMaximum(0);
        b->setMinimum(0);
        b->setValue(0);
    }
}

void ProcessorDlg::slotFinished(const QUrl& url)
{
    qDebug() << "Completed item " << url.toLocalFile();

    QProgressBar* const b = findProgressBar(url);

    if (b)
    {
        b->setMaximum(100);
        b->setMinimum(0);
        b->setValue(100);
        b->setFormat(i18n("Done"));
        d->count--;
        updateCount();
    }
}

void ProcessorDlg::slotFailed(const QUrl& url, const QString& err)
{
    qDebug() << "Failed to complete item " << url.toLocalFile();

    QProgressBar* const b = findProgressBar(url);

    if (b)
    {
        b->setMaximum(100);
        b->setMinimum(0);
        b->setValue(100);
        b->setFormat(err);
        d->count--;
        updateCount();
    }
}
