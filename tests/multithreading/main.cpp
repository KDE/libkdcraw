/** ===========================================================
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * Date        : 2011-12-28
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

// Qt includes

#include <QApplication>
#include <QDebug>

// KDE includes

#include <kurl.h>

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

    KUrl::List list;

    for (int i = 0 ; i < argc ; i++)
        list.append(KUrl(argv[i]));

    ProcessorDlg* const dlg = new ProcessorDlg(list);
    dlg->show();
    app.exec();
    return 0;
}
