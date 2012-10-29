# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-12-09
# @brief  a tread-safe libraw C++ program interface for KDE
#
# @author Copyright (C) 2006-2012 by Gilles Caulier
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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libraw)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

SET(kdcraw_LIB_SRCS kdcraw.cpp
                    kdcraw_p.cpp
                    dcrawsettingswidget.cpp
                    dcrawinfocontainer.cpp
                    rawdecodingsettings.cpp
                    rcombobox.cpp
                    rnuminput.cpp
                    rexpanderbox.cpp
                    ractionthreadbase.cpp
                    ractionthreadbase_p.cpp
                    squeezedcombobox.cpp
   )

KDE4_ADD_LIBRARY(kdcraw SHARED ${kdcraw_LIB_SRCS})

TARGET_LINK_LIBRARIES(kdcraw
                      libraw
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_SOLID_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                     )

SET_TARGET_PROPERTIES(kdcraw PROPERTIES VERSION ${DCRAW_LIB_SO_VERSION_STRING} SOVERSION ${DCRAW_LIB_SO_CUR_VERSION}
                                        DEFINE_SYMBOL LIBRAW_BUILDLIB)

INSTALL(TARGETS kdcraw ${INSTALL_TARGETS_DEFAULT_ARGS})

# Check if compilation is done through digiKam SC or not. If yes, do not install header files.
IF(NOT DIGIKAMSC_USE_PRIVATE_KDEGRAPHICS)
    INSTALL(FILES rawdecodingsettings.h
                  kdcraw.h
                  dcrawsettingswidget.h
                  rnuminput.h
                  rcombobox.h
                  rexpanderbox.h
                  ractionthreadbase.h
                  squeezedcombobox.h
                  dcrawinfocontainer.h
                  rawfiles.h
                  libkdcraw_export.h
                  ${CMAKE_CURRENT_BINARY_DIR}/version.h
            DESTINATION ${INCLUDE_INSTALL_DIR}/libkdcraw COMPONENT Devel)
ENDIF()
