/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2014-10-17
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2014 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "processordlg.moc"

// Qt includes

#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QThreadPool>

// KDE includes

#include <klocale.h>
#include <kdebug.h>
#include <kapplication.h>
#include <kpushbutton.h>

// Local includes

#include "actionthread.h"

class ProcessorDlg::Private
{
public:

    Private()
    {
        page   = 0;
        vlay   = 0;
        thread = 0;
    }

    QWidget*             page;

    QVBoxLayout*         vlay;

    KUrl::List           list;

    ActionThread*        thread;
};

ProcessorDlg::ProcessorDlg(const KUrl::List& list)
    : KDialog(0), d(new Private)
{
    setButtons(Apply | Close);
    setButtonText(Apply, i18n("Start"));
    setDefaultButton(Close);
    setModal(false);

    d->list                 = list;
    d->page                 = new QWidget(this);
    setMainWidget(d->page);

    d->vlay = new QVBoxLayout(d->page);
    QLabel* const pid       = new QLabel(QString("PID : %1")
                                         .arg(kapp->applicationPid()), this);
    QLabel* const core      = new QLabel(QString("Core : %1")
                                         .arg(QThreadPool::globalInstance()->maxThreadCount()), this);
    d->vlay->addWidget(pid);
    d->vlay->addWidget(core);

    for (int i = 0 ; i < d->list.count() ; i++)
    {
        QProgressBar* const bar = new QProgressBar(this);
        bar->setRange(0, 100);
        bar->setObjectName(d->list[i].toLocalFile());
        d->vlay->addWidget(bar);
    }

    d->thread = new ActionThread(this);

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotStart()));

    connect(d->thread, SIGNAL(starting(KUrl)),
            this, SLOT(slotStarting(KUrl)));

    connect(d->thread, SIGNAL(finished(KUrl)),
            this, SLOT(slotFinished(KUrl)));

    connect(d->thread, SIGNAL(failed(KUrl,QString)),
            this, SLOT(slotFailed(KUrl,QString)));
}

ProcessorDlg::~ProcessorDlg()
{
    delete d;
}

void ProcessorDlg::slotStart()
{
    if (d->list.isEmpty()) return;

    button(Apply)->setDisabled(true);

    d->thread->convertRAWtoPNG(d->list);
    d->thread->start();
}

QProgressBar* ProcessorDlg::findProgressBar(const KUrl& url) const
{
    QList<QProgressBar*> bars = d->vlay->findChildren<QProgressBar*>();

    foreach(QProgressBar* const b, bars)
    {
        if (b->objectName() == url.toLocalFile())
        {
            return b;
        }
    }

    return 0;
}

void ProcessorDlg::slotStarting(const KUrl& url)
{
    QProgressBar* const b = findProgressBar(url);

    if (b)
    {
        b->setMaximum(0);
        b->setMinimum(0);
        b->setValue(0);
    }
}

void ProcessorDlg::slotFinished(const KUrl& url)
{
    QProgressBar* const b = findProgressBar(url);

    if (b)
    {
        b->setMaximum(100);
        b->setMinimum(0);
        b->setValue(100);
    }
}

void ProcessorDlg::slotFailed(const KUrl& url, const QString& err)
{
    QProgressBar* const b = findProgressBar(url);

    if (b)
    {
        b->setMaximum(100);
        b->setMinimum(0);
        b->setValue(100);
        b->setFormat(err);
    }
}
