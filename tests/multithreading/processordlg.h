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

#ifndef PROCESSOR_DLG_H
#define PROCESSOR_DLG_H

// KDE includes

#include <QList>
#include <QUrl>
#include <QtWidgets/QDialog>

class QProgressBar;

class ProcessorDlg : public QDialog
{
    Q_OBJECT

public:

    ProcessorDlg(const QList<QUrl> &list);
    ~ProcessorDlg();

private :

    QProgressBar* findProgressBar(const QUrl& url) const;
    void updateCount();

private Q_SLOTS:

    void slotStart();
    void slotStarting(const QUrl&);
    void slotFinished(const QUrl&);
    void slotFailed(const QUrl&, const QString&);

private:

    class Private;
    Private* const d;
};

#endif // PROCESSOR_DLG_H
