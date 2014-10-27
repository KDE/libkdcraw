# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-12-09
# @brief  a tread-safe libraw C++ program interface for KDE
#
# @author Copyright (C) 2006-2014 by Gilles Caulier
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

kde_enable_exceptions()

include_directories(${LibRaw_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

add_definitions(${LibRaw_DEFINITIONS})

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

SET(kdcraw_LIB_SRCS kdcraw.cpp
                    kdcraw_p.cpp
                    dcrawsettingswidget.cpp
                    dcrawinfocontainer.cpp
                    rawdecodingsettings.cpp
                    rcombobox.cpp
                    rnuminput.cpp
                    rexpanderbox.cpp
                    squeezedcombobox.cpp
#TODO                    ractionthreadbase.cpp
#TODO                    ractionthreadbase_p.cpp
)

ecm_generate_headers(kdcraw_CamelCase_HEADERS

                     HEADER_NAMES
                     KDcraw
                     DcrawSettingsWidget
                     DcrawInfoContainer
                     RawDecodingSettings
                     RComboBox
                     RNumInput
                     RExpanderBox
                     SqueezedComboBox
                     RActionThreadBase

                     PREFIX           KDCRAW
                     REQUIRED_HEADERS kdcraw_HEADERS
)

install(FILES       ${kdcraw_CamelCase_HEADERS}
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KDCRAW/KExiv2
        COMPONENT   Devel
)

install(FILES       ${kdcraw_HEADERS}
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KDCRAW/kdcraw
        COMPONENT   Devel
)

add_library(KF5Kdcraw SHARED ${kdcraw_LIB_SRCS})

generate_export_header(KF5Kdcraw BASE_NAME kdcraw EXPORT_MACRO_NAME LIBKDCRAW_EXPORT)

install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/kdcraw_export.h
        DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KDCRAW
        COMPONENT   Devel
)

add_library(KF5::Kdcraw ALIAS KF5Kdcraw)

target_include_directories(KF5Kdcraw INTERFACE 
                           "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KDCRAW;${KF5_INCLUDE_INSTALL_DIR}/KDCRAW/kdcraw>"
)

target_link_libraries(KF5Kdcraw
                      KF5::KDELibs4Support
                      KF5::ThreadWeaver
                      Qt5::Core
                      ${LibRaw_LIBRARIES}
)

install(TARGETS KF5Kdcraw
        EXPORT  KF5KdcrawTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

set_target_properties(KF5Kdcraw   PROPERTIES
                      VERSION     ${KDCRAW_VERSION_STRING}
                      SOVERSION   ${KDCRAW_SOVERSION}
                      EXPORT_NAME Kdcraw
)
