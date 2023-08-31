#
# SPDX-FileCopyrightText: 2010-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(CMAKE_MIN_VERSION   "3.16.0")
set(ECM_MIN_VERSION     "5.91.0")
set(QT_MIN_VERSION      "5.15.0")
set(LIBRAW_MIN_VERSION  "0.18")

cmake_minimum_required(VERSION ${CMAKE_MIN_VERSION})

project(libkdcraw)

message(STATUS "----------------------------------------------------------------------------------")
message(STATUS "Starting CMake configuration for: ${PROJECT_NAME}")

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
# 5.0.0 => 5      (Released with KDE Applications)

# Library API version
SET(KDCRAW_LIB_MAJOR_VERSION "5")
SET(KDCRAW_LIB_MINOR_VERSION "0")
SET(KDCRAW_LIB_PATCH_VERSION "0")

SET(LIBKDCRAW_LIB_VERSION "${KDCRAW_LIB_MAJOR_VERSION}.${KDCRAW_LIB_MINOR_VERSION}.${KDCRAW_LIB_PATCH_VERSION}")
SET(LIBKDCRAW_SO_VERSION   5)

############## ECM setup ######################

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${libkdcraw_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMGenerateHeaders)
include(ECMSetupVersion)
include(CMakePackageConfigHelpers)
include(ECMQtDeclareLoggingCategory)
include(ECMGenerateExportHeader)
include(ECMDeprecationSettings)
include(FeatureSummary)

############## Find Packages ###################

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED NO_MODULE
    COMPONENTS
        Core
        Gui
)

find_package(LibRaw ${LIBRAW_MIN_VERSION} REQUIRED)
set_package_properties("LibRaw"    PROPERTIES
    DESCRIPTION "Required to build libkdcraw"
    URL         "https://www.libraw.org"
    TYPE        RECOMMENDED
    PURPOSE     "Library to decode RAW image"
)

############## Targets #########################

ecm_set_disabled_deprecation_versions(
    QT 5.15
)

add_definitions(
    -DQT_NO_KEYWORDS
    -DQT_NO_FOREACH
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
)

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
