#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

kde_enable_exceptions()

include_directories(${LibRaw_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

add_definitions(${LibRaw_DEFINITIONS})

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII)

set(kdcraw_LIB_SRCS kdcraw.cpp
                    kdcraw_p.cpp
                    dcrawsettingswidget.cpp
                    dcrawinfocontainer.cpp
                    rawdecodingsettings.cpp
                    rcombobox.cpp
                    rnuminput.cpp
                    rexpanderbox.cpp
                    squeezedcombobox.cpp
                    ractionthreadbase.cpp
                    ractionjob.cpp
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
                     RActionJob

                     PREFIX           KDCRAW
                     REQUIRED_HEADERS kdcraw_HEADERS
)

add_library(KF5KDcraw SHARED ${kdcraw_LIB_SRCS})
add_library(KF5::KDcraw ALIAS KF5KDcraw)

generate_export_header(KF5KDcraw BASE_NAME kdcraw EXPORT_MACRO_NAME LIBKDCRAW_EXPORT)

# Check if compilation is done through digiKam SC or not. If yes, do not install header files.
if (NOT DIGIKAMSC_COMPILE_LIBKDCRAW)

    install(FILES       ${kdcraw_CamelCase_HEADERS}
            DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KDCRAW/KDCRAW
            COMPONENT   Devel
    )

    install(FILES       ${kdcraw_HEADERS}
            DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KDCRAW/kdcraw
            COMPONENT   Devel
    )

    install(FILES       ${CMAKE_CURRENT_BINARY_DIR}/kdcraw_export.h
            DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KDCRAW
            COMPONENT   Devel
    )
    
    install(FILES       "${CMAKE_CURRENT_BINARY_DIR}/libkdcraw_version.h"
            DESTINATION "${KF5_INCLUDE_INSTALL_DIR}"
            COMPONENT   Devel
    )

endif()

target_include_directories(KF5KDcraw INTERFACE
                           "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KDCRAW;${KF5_INCLUDE_INSTALL_DIR}/KDCRAW/kdcraw>"
)

target_link_libraries(KF5KDcraw
                      PRIVATE
                      ${LibRaw_LIBRARIES}
                      PUBLIC
                      KF5::KDELibs4Support
                      Qt5::Core
)

install(TARGETS KF5KDcraw
        EXPORT  KF5KDcrawTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
)

set_target_properties(KF5KDcraw   PROPERTIES
                      VERSION     ${KDCRAW_VERSION_STRING}
                      SOVERSION   ${KDCRAW_SOVERSION}
                      EXPORT_NAME KDcraw
)
