# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2014-10-17
# @brief  threadweaver api test application
#
# @author Copyright (C) 2014 by Gilles Caulier
#         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# ============================================================

INCLUDE_DIRECTORIES(${KDE4_INCLUDES}
                    ${QT_INCLUDE_DIR}
                    ${QT_QTCORE_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../libkdcraw
)

SET(multicorerawtopng_SRCS
    main.cpp
    processordlg.cpp
    actionthread.cpp
)

KDE4_ADD_EXECUTABLE(multicorerawtopng ${multicorerawtopng_SRCS})

TARGET_LINK_LIBRARIES(multicorerawtopng
                      ${QT_QTGUI_LIBRARY}
                      ${QT_QTCORE_LIBRARY}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      kdcraw
)
