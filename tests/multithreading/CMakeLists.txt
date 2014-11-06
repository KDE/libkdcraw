#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include_directories(${KDE4_INCLUDES}
                    ${QT_INCLUDE_DIR}
                    ${QT_QTCORE_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../libkdcraw
                   )

set(multicorerawtopng_SRCS
    main.cpp
    processordlg.cpp
    actionthread.cpp
   )

KDE4_ADD_EXECUTABLE(multicorerawtopng ${multicorerawtopng_SRCS})

target_link_libraries(multicorerawtopng
                      ${QT_QTGUI_LIBRARY}
                      ${QT_QTCORE_LIBRARY}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      kdcraw
                     )
