PROJECT(libkdcraw)

# =======================================================
# Informations to update before to release this library.

# Library version history:
# API      ABI
# 0.1.0 => 0.1.0  (KDE3)
# 0.1.1 => 1.0.0  (KDE3)
# 0.1.2 => 2.0.0  (KDE3)
# 0.1.3 => 2.1.0  (KDE3)
# 0.1.4 => 3.0.0  (KDE3)
# 0.1.5 => 4.0.0  (KDE3)
# 0.2.0 => 5.0.0  (released with KDE 4.1.0)
# 0.3.0 => 6.0.0  (released with KDE 4.2.0)

# Library API version
SET(DCRAW_LIB_MAJOR_VERSION "0")
SET(DCRAW_LIB_MINOR_VERSION "3")
SET(DCRAW_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(DCRAW_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual.html#Updating-version-info
SET(DCRAW_LIB_SO_CUR_VERSION "6")
SET(DCRAW_LIB_SO_REV_VERSION "0")
SET(DCRAW_LIB_SO_AGE_VERSION "0")

# dcraw.c version include with this library. 
# Look into dcraw/dcraw.c implementation for details.
SET(DCRAW_PROGRAM_VERSION "8.87")

# =======================================================
# Set env. variables accordinly.

SET(DCRAW_PROGRAM_DIR "${LIB_INSTALL_DIR}/libkdcraw${DCRAW_LIB_SO_CUR_VERSION}")

SET(DCRAW_LIB_VERSION_STRING "${DCRAW_LIB_MAJOR_VERSION}.${DCRAW_LIB_MINOR_VERSION}.${DCRAW_LIB_PATCH_VERSION}${DCRAW_LIB_SUFFIX_VERSION}")
SET(DCRAW_LIB_VERSION_ID "0x0${DCRAW_LIB_MAJOR_VERSION}0${DCRAW_LIB_MINOR_VERSION}0${DCRAW_LIB_PATCH_VERSION}")

SET(DCRAW_LIB_SO_VERSION_STRING "${DCRAW_LIB_SO_CUR_VERSION}.${DCRAW_LIB_SO_REV_VERSION}.${DCRAW_LIB_SO_AGE_VERSION}")

# =======================================================

# Depencies detection require to compile dcraw.c
# Detect Little CMS library properlly with CMake script from 'cmake/modules'
FIND_PACKAGE(LCMS REQUIRED)

# Detect libJPEG
FIND_PACKAGE(JPEG REQUIRED)

# LibRAW source code -------------------------------------------------------------

ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libraw)

SET(libraw_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/dcraw_common.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/foveon.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_cxx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_c_api.cpp
   )

# --------------------------------------------------------------------------------

SUBDIRS(icons)
SUBDIRS(libkdcraw)
SUBDIRS(profiles)

IF(NOT WIN32)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.lsm)
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
ENDIF(NOT WIN32)
