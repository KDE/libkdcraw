#
# SPDX-FileCopyrightText: 2010-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

kde_enable_exceptions()

include_directories(SYSTEM ${LibRaw_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

add_definitions(${LibRaw_DEFINITIONS})

set(kdcraw_LIB_SRCS
    kdcraw.cpp
    kdcraw_p.cpp
    dcrawinfocontainer.cpp
    rawdecodingsettings.cpp
)

ecm_qt_declare_logging_category(kdcraw_LIB_SRCS
    HEADER libkdcraw_debug.h
    IDENTIFIER LIBKDCRAW_LOG
    CATEGORY_NAME "libkdcraw"
    DESCRIPTION "libkdcraw"
    EXPORT KDCRAW
)

ecm_generate_headers(kdcraw_CamelCase_HEADERS
    HEADER_NAMES
        KDcraw
        DcrawInfoContainer
        RawDecodingSettings
        RawFiles
    PREFIX KDCRAW
    REQUIRED_HEADERS kdcraw_HEADERS
)

add_library(KF5KDcraw SHARED ${kdcraw_LIB_SRCS})
add_library(KF5::KDcraw ALIAS KF5KDcraw)

generate_export_header(KF5KDcraw
    BASE_NAME libkdcraw
)

set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE OFF)

target_include_directories(KF5KDcraw
    INTERFACE
        "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KDCRAW;${KF5_INCLUDE_INSTALL_DIR}/KDCRAW/kdcraw>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

target_link_libraries(KF5KDcraw
    PUBLIC
        Qt5::Core
        Qt5::Gui
    PRIVATE
        ${LibRaw_LIBRARIES}
)

set_target_properties(KF5KDcraw PROPERTIES
    VERSION     ${KDCRAW_VERSION_STRING}
    SOVERSION   ${KDCRAW_SOVERSION}
    EXPORT_NAME KDcraw
)

install(TARGETS KF5KDcraw
    EXPORT  KF5KDcrawTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

install(FILES 
    ${kdcraw_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KDCRAW/KDCRAW
    COMPONENT   Devel
)

install(FILES
    ${kdcraw_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KDCRAW/kdcraw
    COMPONENT   Devel
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/libkdcraw_version.h"
    DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF5}"
    COMPONENT   Devel
)

ecm_qt_install_logging_categories(
    EXPORT KDCRAW
    FILE libkdcraw.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)
