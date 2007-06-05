project(libkdcraw)

subdirs(icons)
subdirs(dcraw)
subdirs(src)

# Depencies detection require to compile dcraw.c
# Detect Little CMS library properlly with CMake script from 'cmake/modules'
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
find_package(LCMS REQUIRED)

# Detect libJPEG
find_package(JPEG REQUIRED)

include_directories(${KDE4_INCLUDES} ${QT4_INCLUDES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
