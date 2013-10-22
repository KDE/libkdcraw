# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-12-09
# @brief  a tread-safe libraw C++ program interface for KDE
#
# @author Copyright (C) 2006-2013 by Gilles Caulier
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

SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

MESSAGE(STATUS "----------------------------------------------------------------------------------")
MESSAGE(STATUS "Starting CMake configuration for: libkdcraw")

FIND_PACKAGE(KDE4 REQUIRED)
INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)
INCLUDE(MacroOptionalAddSubdirectory)
INCLUDE(FindPackageHandleStandardArgs)

# NOTE: Libraw 0.16.x is prefered version to use because it's ported to Cmake with full features supported.
# Until libraw 0.16.0 is release (ends of 2013), we will support previous version (with limited support)
FIND_PACKAGE(LibRaw 0.15 REQUIRED)

#Check LibRaw Major and Minor version to rules compilation.
MACRO_ENSURE_VERSION("0.16.0" "${LibRaw_VERSION_STRING}" VERSION_LIBRAW_WITH_CMAKE_SUPPORT)
MESSAGE(STATUS "LibRaw version: ${LibRaw_VERSION_STRING} - has config file: ${VERSION_LIBRAW_WITH_CMAKE_SUPPORT} - LDFLAGS: ${LibRaw_r_LDFLAGS}")

IF(VERSION_LIBRAW_WITH_CMAKE_SUPPORT)
    ADD_DEFINITIONS(-DLIBRAW_HAS_CONFIG)
ENDIF()

ADD_DEFINITIONS(${QT_DEFINITIONS}
                ${QT_QTDBUS_DEFINITIONS}
                ${KDE4_DEFINITIONS}
                ${LibRaw_r_DEFINITIONS}
               )

INCLUDE_DIRECTORIES(${QDBUS_INCLUDE_DIRS} 
                    ${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${KDE4_INCLUDES}
                    ${LibRaw_INCLUDE_DIR}
                   )

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
# 1.2.0 => 9.0.0  (Released with KDE 4.5.0)
# 1.3.0 => 10.0.0 (Released with KDE 4.6.0)
# 2.0.0 => 20.0.0 (Released with KDE 4.7.0)
# 2.0.1 => 20.0.1 (Released with KDE 4.8.0  - Introducing new deprecated members due to use libraw 0.14.x)
# 2.1.0 => 21.0.0 (Released with KDE 4.8.1  - Remove deprecated members)
# 2.2.0 => 22.0.0 (Released with KDE 4.10.0 - Including RawSpeed Codec)
# 2.3.0 => 22.1.0 (Released with KDE 4.11.0 - Including Coverity Report fixes)
# 2.3.1 => 22.1.1 (Released with KDE 4.11.2 - Including settings widget fixes)
# 2.4.0 => 23.0.0 (Released with KDE 4.12.0 - Drop internal Libraw source code)

# Library API version
SET(DCRAW_LIB_MAJOR_VERSION "2")
SET(DCRAW_LIB_MINOR_VERSION "4")
SET(DCRAW_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(DCRAW_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
SET(DCRAW_LIB_SO_CUR_VERSION "23")
SET(DCRAW_LIB_SO_REV_VERSION "0")
SET(DCRAW_LIB_SO_AGE_VERSION "0")

# ==================================================================================================
# Set env. variables accordinly.

SET(DCRAW_LIB_VERSION_STRING "${DCRAW_LIB_MAJOR_VERSION}.${DCRAW_LIB_MINOR_VERSION}.${DCRAW_LIB_PATCH_VERSION}${DCRAW_LIB_SUFFIX_VERSION}")
SET(DCRAW_LIB_VERSION_ID "0x0${DCRAW_LIB_MAJOR_VERSION}0${DCRAW_LIB_MINOR_VERSION}0${DCRAW_LIB_PATCH_VERSION}")

SET(DCRAW_LIB_SO_VERSION_STRING "${DCRAW_LIB_SO_CUR_VERSION}.${DCRAW_LIB_SO_REV_VERSION}.${DCRAW_LIB_SO_AGE_VERSION}")

# ==================================================================================================

ADD_SUBDIRECTORY(libkdcraw)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(icons)
ADD_SUBDIRECTORY(profiles)

IF(NOT WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake  ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.lsm)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
ENDIF(NOT WIN32)

MACRO_DISPLAY_FEATURE_LOG()
