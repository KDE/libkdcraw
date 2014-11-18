/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date  : 2011-12-28
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

// Qt includes

#include <QApplication>
#include <QDebug>

// KDE includes

#include <QUrl>

// Local includes

#include "processordlg.h"

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        qDebug() << "multicoreraw2png - RAW Camera Image to PNG Converter using multi-core CPU";
        qDebug() << "Usage: multicoreraw2png <rawfile> <rawfile> <rawfile> ... <rawfile>";
        return -1;
    }

    QApplication app(argc, argv);

    QList<QUrl> list;

    for (int i = 1 ; i < argc ; i++)
        list.append(QUrl::fromLocalFile(argv[i]));

    ProcessorDlg* const dlg = new ProcessorDlg(list);
    dlg->show();
    app.exec();
    return 0;
}
