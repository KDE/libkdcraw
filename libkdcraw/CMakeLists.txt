# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-12-09
# @brief  a tread-safe libraw C++ program interface for KDE
#
# @author Copyright (C) 2006-2010 by Gilles Caulier
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

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libraw_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/libraw_config.h)

# Disable dcraw C Ansi code warnings from LibRaw. Just to be clear on the console.
FOREACH(_curentfile ${libraw_LIB_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w )
ENDFOREACH(_curentfile ${libraw_LIB_SRCS})

SET(kdcraw_LIB_SRCS kdcraw.cpp
                    kdcraw_p.cpp
                    dcrawsettingswidget.cpp
                    dcrawinfocontainer.cpp
                    rawdecodingsettings.cpp
                    rcombobox.cpp
                    rnuminput.cpp
                    rexpanderbox.cpp
                    squeezedcombobox.cpp
                    ${libraw_LIB_SRCS}
   )

KDE4_ADD_LIBRARY(kdcraw SHARED ${kdcraw_LIB_SRCS})

TARGET_LINK_LIBRARIES(kdcraw
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${MATH_LIBRARY}
                      ${LCMS_LIBRARIES}
                      ${JPEG_LIBRARY}
                      ${OPENMP_LDFLAGS}
                     )

SET_TARGET_PROPERTIES(kdcraw PROPERTIES VERSION ${DCRAW_LIB_SO_VERSION_STRING} SOVERSION ${DCRAW_LIB_SO_CUR_VERSION}
                                        DEFINE_SYMBOL LIBRAW_BUILDLIB)

INSTALL(TARGETS kdcraw ${INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(FILES rawdecodingsettings.h
              kdcraw.h
              dcrawsettingswidget.h
              rnuminput.h
              rcombobox.h
              rexpanderbox.h
              squeezedcombobox.h
              dcrawinfocontainer.h
              rawfiles.h
              libkdcraw_export.h
              ${CMAKE_CURRENT_BINARY_DIR}/version.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkdcraw COMPONENT Devel)
