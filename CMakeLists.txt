PROJECT(libkdcraw)

# =======================================================
# Informations to update before to release this library.

# Library version history:
# API      BIN
# 0.1.0 => 0.1.0
# 0.1.1 => 1.0.0
# 0.1.2 => 1.1.0
# 0.2.0 => 2.0.0

# Library API version
SET(DCRAW_LIB_MAJOR_VERSION "0")
SET(DCRAW_LIB_MINOR_VERSION "2")
SET(DCRAW_LIB_PATCH_VERSION "0")

# Library BIN version used by linker.
# For details : http://www.gnu.org/software/libtool/manual.html#Updating-version-info
SET(DCRAW_LIB_SO_CUR_VERSION "2")
SET(DCRAW_LIB_SO_REV_VERSION "0")
SET(DCRAW_LIB_SO_AGE_VERSION "0")

# dcraw.c version include with this library. 
# Look into dcraw/dcraw.c implementation for details.
SET(DCRAW_PROGRAM_VERSION "8.77")

# =======================================================
# Set env. variables accordinly.

SET(DCRAW_PROGRAM_DIR "${LIB_INSTALL_DIR}/libkdcraw${DCRAW_LIB_SO_CUR_VERSION}")

SET(DCRAW_LIB_VERSION_STRING "${DCRAW_LIB_MAJOR_VERSION}.${DCRAW_LIB_MINOR_VERSION}.${DCRAW_LIB_PATCH_VERSION}")
SET(DCRAW_LIB_VERSION_ID "0x0${DCRAW_LIB_MAJOR_VERSION}0${DCRAW_LIB_MINOR_VERSION}0${DCRAW_LIB_PATCH_VERSION}")

SET(DCRAW_LIB_SO_VERSION_STRING "${DCRAW_LIB_SO_CUR_VERSION}.${DCRAW_LIB_SO_REV_VERSION}.${DCRAW_LIB_SO_AGE_VERSION}")

# =======================================================

# Depencies detection require to compile dcraw.c
# Detect Little CMS library properlly with CMake script from 'cmake/modules'
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
FIND_PACKAGE(LCMS REQUIRED)

# Detect libJPEG
FIND_PACKAGE(JPEG REQUIRED)

SUBDIRS(icons)
SUBDIRS(libkdcraw)
SUBDIRS(dcraw)

IF(NOT WIN32)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.lsm)
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
ENDIF(NOT WIN32)
