#
# SPDX-FileCopyrightText: 2010-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(KDCRAW_INSTALL_INCLUDEDIR "${KDE_INSTALL_INCLUDEDIR_KF5}/KDCRAW")
set(KDCRAW_CMAKECONFIG_NAME "KF5KDcraw")

ecm_setup_version(${LIBKDCRAW_LIB_VERSION}
    VARIABLE_PREFIX      KDCRAW
    VERSION_HEADER       "libkdcraw_version.h"
    PACKAGE_VERSION_FILE "${KDCRAW_CMAKECONFIG_NAME}ConfigVersion.cmake"
    SOVERSION            ${LIBKDCRAW_SO_VERSION}
)

kde_enable_exceptions()

add_library(KDcraw SHARED)

target_sources(KDcraw PRIVATE
    kdcraw.cpp
    kdcraw_p.cpp
    dcrawinfocontainer.cpp
    rawdecodingsettings.cpp
)

ecm_qt_declare_logging_category(KDcraw
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

generate_export_header(KDcraw
    BASE_NAME libkdcraw
)

target_include_directories(KDcraw
    INTERFACE
        "$<INSTALL_INTERFACE:${KDCRAW_INSTALL_INCLUDEDIR};${KDCRAW_INSTALL_INCLUDEDIR}/kdcraw>"
)

target_link_libraries(KDcraw
    PUBLIC
        Qt5::Core
        Qt5::Gui
    PRIVATE
        LibRaw::LibRaw
)

set_target_properties(KDcraw PROPERTIES
    OUTPUT_NAME KF5KDcraw
    VERSION     ${KDCRAW_VERSION}
    SOVERSION   ${KDCRAW_SOVERSION}
)

install(TARGETS KDcraw
    EXPORT KDcrawTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

install(FILES 
    ${kdcraw_CamelCase_HEADERS}
    DESTINATION ${KDCRAW_INSTALL_INCLUDEDIR}/KDCRAW
    COMPONENT   Devel
)

install(FILES
    ${kdcraw_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw_export.h
    DESTINATION ${KDCRAW_INSTALL_INCLUDEDIR}/kdcraw
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

############## CMake Config Files ##############

message("${CMAKECONFIG_INSTALL_DIR}")
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/${KDCRAW_CMAKECONFIG_NAME}")

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/templates/${KDCRAW_CMAKECONFIG_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${KDCRAW_CMAKECONFIG_NAME}Config.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${KDCRAW_CMAKECONFIG_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${KDCRAW_CMAKECONFIG_NAME}ConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT   Devel
)

install(EXPORT KDcrawTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE        ${KDCRAW_CMAKECONFIG_NAME}Targets.cmake
    NAMESPACE   KF5::
)
