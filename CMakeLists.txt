project(libkdcraw)

subdirs(icons)
subdirs(dcraw)

# DEpencies detection require to compile dcraw.c
# Detect Little CMS library properlly with CMake script from 'cmake/modules'
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
find_package(LCMS REQUIRED)

# Detect libJPEG
find_package(JPEG REQUIRED)

include_directories(${KDE4_INCLUDES} ${QT4_INCLUDES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)

########### next target ###############

set(kdcraw_LIB_SRCS dcrawbinary.cpp kdcraw.cpp dcrawsettingswidget.cpp)

kde4_automoc(${kdcraw_LIB_SRCS})

kde4_add_library(kdcraw SHARED ${kdcraw_LIB_SRCS})

target_link_libraries(kdcraw ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} lcms)

set_target_properties(kdcraw PROPERTIES VERSION 2.0.0 SOVERSION 2)

install(TARGETS kdcraw DESTINATION ${LIB_INSTALL_DIR})

########### install files ###############

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)

install(FILES rawdecodingsettings.h kdcraw.h dcrawbinary.h dcrawsettingswidget.h dcrawinfocontainer.h rawfiles.h libkdcraw_export.h version.h DESTINATION ${INCLUDE_INSTALL_DIR}/libkdcraw)

