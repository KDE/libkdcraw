#
# Copyright (c) 2010-2014, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)

set(kdcraw_LIB_SRCS kdcraw.cpp
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

target_link_libraries(kdcraw
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${KDE4_SOLID_LIBS}
                      ${KDE4_THREADWEAVER_LIBRARIES}
                      ${LibRaw_r_LIBRARIES}
                     )

set_target_properties(kdcraw PROPERTIES VERSION ${DCRAW_LIB_SO_VERSION_STRING} SOVERSION ${DCRAW_LIB_SO_CUR_VERSION})

install(TARGETS kdcraw ${INSTALL_TARGETS_DEFAULT_ARGS})

# Check if compilation is done through digiKam SC or not. If yes, do not install header files.
if(NOT DIGIKAMSC_COMPILE_LIBKDCRAW)
    install(FILES rawdecodingsettings.h
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

    if(NOT WIN32)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/libkdcraw.pc.cmake.in  ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/templates/libkdcraw.lsm.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.lsm)
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
    endif()
endif()
