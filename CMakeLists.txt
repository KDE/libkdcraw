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

PROJECT(libkdcraw)

FIND_PACKAGE(KDE4 REQUIRED)
INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)
INCLUDE(MacroOptionalAddSubdirectory)
INCLUDE(FindPackageHandleStandardArgs)

ADD_DEFINITIONS (${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
INCLUDE_DIRECTORIES (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

SET(LIBKDCRAW_AREA_CODE_GENERAL 51002)
ADD_DEFINITIONS(-DKDE_DEFAULT_DEBUG_AREA=${LIBKDCRAW_AREA_CODE_GENERAL})

# ==================================================================================================
# Information to update before to release this library.

# Library version history:
# API      ABI
# 0.1.0 => 0.1.0  (KDE3)
# 0.1.1 => 1.0.0  (KDE3)
# 0.1.2 => 2.0.0  (KDE3)
# 0.1.3 => 2.1.0  (KDE3)
# 0.1.4 => 3.0.0  (KDE3)
# 0.1.5 => 4.0.0  (KDE3)
# 0.2.0 => 5.0.0  (released with KDE 4.1.0)
# 0.3.0 => 6.0.0  (released with KDE 4.1.2)
# 0.4.0 => 7.0.0  (using libRaw-Released with KDE 4.2.0)
# 0.5.0 => 7.1.0  (Released with KDE 4.3.0)
# 1.0.0 => 8.0.0  (Released with KDE 4.4.0)
# 1.1.0 => 8.1.0  (Released with KDE 4.4.2)
# 1.2.0 => 9.0.0  (Released with KDE 4.6.0)

# Library API version
SET(DCRAW_LIB_MAJOR_VERSION "1")
SET(DCRAW_LIB_MINOR_VERSION "2")
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
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
SET(DCRAW_LIB_SO_CUR_VERSION "9")
SET(DCRAW_LIB_SO_REV_VERSION "0")
SET(DCRAW_LIB_SO_AGE_VERSION "0")

# ==================================================================================================
# Set env. variables accordinly.

SET(DCRAW_LIB_VERSION_STRING "${DCRAW_LIB_MAJOR_VERSION}.${DCRAW_LIB_MINOR_VERSION}.${DCRAW_LIB_PATCH_VERSION}${DCRAW_LIB_SUFFIX_VERSION}")
SET(DCRAW_LIB_VERSION_ID "0x0${DCRAW_LIB_MAJOR_VERSION}0${DCRAW_LIB_MINOR_VERSION}0${DCRAW_LIB_PATCH_VERSION}")

SET(DCRAW_LIB_SO_VERSION_STRING "${DCRAW_LIB_SO_CUR_VERSION}.${DCRAW_LIB_SO_REV_VERSION}.${DCRAW_LIB_SO_AGE_VERSION}")

# ==================================================================================================
# LibRAW source code

IF(NOT WIN32)
    FIND_LIBRARY(MATH_LIBRARY m)
ENDIF(NOT WIN32)

FIND_PACKAGE(LCMS REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)

# Flag to compile Little CMS code from LibRaw
ADD_DEFINITIONS(-DUSE_LCMS)

# To prevent warnings from M$ compiler
IF(WIN32 AND MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32 AND MSVC)

# OpenMP detection (requires CMake >= 2.6.3)
# NOTE: OpenMP under MacOSX do not support multithreading.

IF(NOT APPLE)
    MACRO_OPTIONAL_FIND_PACKAGE(OpenMP)

    IF(OPENMP_FOUND)
        ADD_DEFINITIONS(${OpenMP_CXX_FLAGS})
        IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU )
            SET(OPENMP_LDFLAGS "-lgomp")
        ENDIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU )
        IF("${CMAKE_C_COMPILER_ID}" STREQUAL SunPro )
            SET(OPENMP_LDFLAGS "-xopenmp")
        ENDIF("${CMAKE_C_COMPILER_ID}" STREQUAL SunPro )
    ENDIF(OPENMP_FOUND)
ENDIF(NOT APPLE)

# Flag to debug LibRaw
ADD_DEFINITIONS(-DDCRAW_VERBOSE)

# LibRaw use C++ exception.
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

# Under Windows, use specific flag to compile.
IF(WIN32)
    ADD_DEFINITIONS( -DDJGPP )
ENDIF(WIN32)

# ==================================================================================================

INCLUDE_DIRECTORIES(
                    ${JPEG_INCLUDE_DIR}
                    ${LCMS_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw
                   )

# Do not compile LibRaw with KDE FINAL mode.
KDE4_NO_ENABLE_FINAL(libraw)

SET(libraw_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/dcraw_common.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/dcraw_fileio.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_cxx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_c_api.cpp
   )

# =======================================================

IF(NOT WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.lsm)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
ENDIF(NOT WIN32)

SUBDIRS(icons)
SUBDIRS(libkdcraw)
SUBDIRS(test)
SUBDIRS(profiles)
