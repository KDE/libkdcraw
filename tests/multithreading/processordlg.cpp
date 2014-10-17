/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * Date        : 2014-10-17
 * Description : test for implementation of threadWeaver api
 *
 * Copyright (C) 2014 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
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
#include <QFileInfo>

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
    setCaption(i18n("Convert RAW files To PNG"));

    d->list                 = list;
    d->count                = d->list.count()-1;
    d->page                 = new QWidget(this);
    setMainWidget(d->page);

    d->vlay                 = new QVBoxLayout(d->page);
    QLabel* const pid       = new QLabel(i18n("PID : %1", kapp->applicationPid()), this);
    QLabel* const core      = new QLabel(i18n("Core : %1", QThreadPool::globalInstance()->maxThreadCount()), this);
    d->items                = new QLabel(this);
    d->vlay->addWidget(pid);
    d->vlay->addWidget(core);
    d->vlay->addWidget(d->items);
    d->vlay->addStretch();

    for (int i = 1; i < d->count ; i++)
    {
        QProgressBar* const bar = new QProgressBar(this);
        QString file            = d->list[i].toLocalFile();
        bar->setMaximum(100);
        bar->setMinimum(0);
        bar->setValue(100);
        bar->setObjectName(file);
        QFileInfo fi(file);
        bar->setFormat(fi.baseName());
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

    button(Apply)->setDisabled(true);

    d->thread->convertRAWtoPNG(d->list);
    d->thread->start();
}

QProgressBar* ProcessorDlg::findProgressBar(const KUrl& url) const
{
    QList<QProgressBar*> bars = findChildren<QProgressBar*>();

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
        b->setFormat(i18n("Done"));
        d->count--;
        updateCount();
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
        d->count--;
        updateCount();
    }
}
