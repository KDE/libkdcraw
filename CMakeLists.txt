#
# Copyright (c) 2010-2015, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

cmake_minimum_required(VERSION 2.8.9)
project(libkdcraw)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for: libkdcraw")

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)
include(MacroOptionalAddSubdirectory)
include(FindPackageHandleStandardArgs)

# NOTE: Libraw 0.16.x is prefered version to use because it's ported to Cmake with full features supported.
# Until libraw 0.16.0 is release (ends of 2013), we will support previous version (with limited support)
find_package(LibRaw 0.15)
SET_PACKAGE_PROPERTIES(LibRaw PROPERTIES DESCRIPTION "library that provides image Raw file demosaicing" URL "http://www.libraw.org" TYPE REQUIRED PURPOSE "Required to build libkdcraw")
if(NOT LIBRAW_FOUND)
    FEATURE_SUMMARY(WHAT ALL
                    INCLUDE_QUIET_PACKAGES
                    FATAL_ON_MISSING_REQUIRED_PACKAGES
                   )
    return()
endif()

# Check LibRaw config header.
message(STATUS "LibRaw version: ${LibRaw_VERSION_STRING}")

if(EXISTS "${LibRaw_INCLUDE_DIR}/libraw_config.h")
    add_definitions(-DLIBRAW_HAS_CONFIG)
    message(STATUS "LibRaw config file exists: yes")
else()
    message(STATUS "LibRaw config file exists: no")
endif()

add_definitions(${QT_DEFINITIONS}
                ${QT_QTDBUS_DEFINITIONS}
                ${KDE4_DEFINITIONS}
                ${LibRaw_r_DEFINITIONS}
               )

include_directories(${QDBUS_INCLUDE_DIRS} 
                    ${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${KDE4_INCLUDES}
                    ${LibRaw_INCLUDE_DIR}
                   )

set(LIBKDCRAW_AREA_CODE_GENERAL 51002)
add_definitions(-DKDE_DEFAULT_DEBUG_AREA=${LIBKDCRAW_AREA_CODE_GENERAL})

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
# 2.4.0 => 23.0.2 (Released with KDE 4.12.0 - Drop internal Libraw source code + new methods to get thumb and preview from QBuffer)

# Library API version
set(DCRAW_LIB_MAJOR_VERSION "2")
set(DCRAW_LIB_MINOR_VERSION "4")
set(DCRAW_LIB_PATCH_VERSION "2")

# Suffix to add at end of version string. Usual values are:
# "-git"   : alpha code unstable from git. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
set(DCRAW_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
set(DCRAW_LIB_SO_CUR_VERSION "23")
set(DCRAW_LIB_SO_REV_VERSION "0")
set(DCRAW_LIB_SO_AGE_VERSION "0")

# ==================================================================================================
# Set env. variables accordinly.

set(DCRAW_LIB_VERSION_STRING    "${DCRAW_LIB_MAJOR_VERSION}.${DCRAW_LIB_MINOR_VERSION}.${DCRAW_LIB_PATCH_VERSION}${DCRAW_LIB_SUFFIX_VERSION}")
set(DCRAW_LIB_VERSION_ID        "0x0${DCRAW_LIB_MAJOR_VERSION}0${DCRAW_LIB_MINOR_VERSION}0${DCRAW_LIB_PATCH_VERSION}")
set(DCRAW_LIB_SO_VERSION_STRING "${DCRAW_LIB_SO_CUR_VERSION}.${DCRAW_LIB_SO_REV_VERSION}.${DCRAW_LIB_SO_AGE_VERSION}")

# ==================================================================================================

add_subdirectory(libkdcraw)
add_subdirectory(icons)
add_subdirectory(profiles)

if(KDE4_BUILD_TESTS)
    MACRO_OPTIONAL_ADD_SUBDIRECTORY(tests)
endif()

FEATURE_SUMMARY(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
               )
