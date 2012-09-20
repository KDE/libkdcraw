# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-12-09
# @brief  a tread-safe libraw C++ program interface for KDE
#
# @author Copyright (C) 2006-2012 by Gilles Caulier
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

OPTION(ENABLE_LCMS2    "Link libraw to LCMS2 else LCMS1 (default=OFF)"    OFF)
OPTION(ENABLE_RAWSPEED "Compile libraw with RawSpeed codec (default=OFF)" OFF)

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
# 1.2.0 => 9.0.0  (Released with KDE 4.5.0)
# 1.3.0 => 10.0.0 (Released with KDE 4.6.0)
# 2.0.0 => 20.0.0 (Released with KDE 4.7.0)
# 2.0.1 => 20.0.1 (Released with KDE 4.8.0  - Introducing new deprecated members due to use libraw 0.14.x)
# 2.1.0 => 21.0.0 (Released with KDE 4.8.1  - Remove deprecated members)
# 2.2.0 => 22.0.0 (Released with KDE 4.10.0 - Including RawSpeed Codec)

# Library API version
SET(DCRAW_LIB_MAJOR_VERSION "2")
SET(DCRAW_LIB_MINOR_VERSION "2")
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
SET(DCRAW_LIB_SO_CUR_VERSION "22")
SET(DCRAW_LIB_SO_REV_VERSION "0")
SET(DCRAW_LIB_SO_AGE_VERSION "0")

# ==================================================================================================
# Set env. variables accordinly.

SET(DCRAW_LIB_VERSION_STRING "${DCRAW_LIB_MAJOR_VERSION}.${DCRAW_LIB_MINOR_VERSION}.${DCRAW_LIB_PATCH_VERSION}${DCRAW_LIB_SUFFIX_VERSION}")
SET(DCRAW_LIB_VERSION_ID "0x0${DCRAW_LIB_MAJOR_VERSION}0${DCRAW_LIB_MINOR_VERSION}0${DCRAW_LIB_PATCH_VERSION}")

SET(DCRAW_LIB_SO_VERSION_STRING "${DCRAW_LIB_SO_CUR_VERSION}.${DCRAW_LIB_SO_REV_VERSION}.${DCRAW_LIB_SO_AGE_VERSION}")

# ==================================================================================================
# Compiler optimizations for Release target.

IF (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_C_FLAGS_RELEASE "-O4 -DNDEBUG -DQT_NO_DEBUG")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O4 -DNDEBUG -DQT_NO_DEBUG")
ENDIF (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

# ==================================================================================================
# LibRAW source code

IF(NOT WIN32)
    FIND_LIBRARY(MATH_LIBRARY m)
ENDIF(NOT WIN32)

# LCMS version 1 and 2 library handling
# See B.K.O #294105
IF (ENABLE_LCMS2)
    MESSAGE(STATUS "Check for LCMS2 availability...")
    FIND_PACKAGE(LCMS2)
    IF (LCMS2_FOUND AND (LCMS2_VERSION VERSION_EQUAL 2.1 OR LCMS2_VERSION VERSION_GREATER 2.1))
        MESSAGE(STATUS "Found LCMS2: ${LCMS2_LIBRARIES} ${LCMS2_INCLUDE_DIR}")
        INCLUDE_DIRECTORIES(${LCMS2_INCLUDE_DIR})
        MACRO_LOG_FEATURE(LCMS2_FOUND "LCMS2" "A small-footprint color management engine" "http://www.littlecms.com" FALSE "" "Needed by libkdcraw")
        # Flag to compile Little CMS version 2 with LibRaw
        ADD_DEFINITIONS(-DUSE_LCMS2)
    ENDIF (LCMS2_FOUND AND (LCMS2_VERSION VERSION_EQUAL 2.1 OR LCMS2_VERSION VERSION_GREATER 2.1))
ELSE (ENABLE_LCMS2)
    MESSAGE(STATUS "Check for LCMS1 availability...")
    FIND_PACKAGE(LCMS)
    IF (LCMS_FOUND)
        MESSAGE(STATUS "Found LCMS1: ${LCMS_LIBRARIES} ${LCMS_INCLUDE_DIR}")
        INCLUDE_DIRECTORIES(${LCMS_INCLUDE_DIR})
        MACRO_LOG_FEATURE(LCMS_FOUND "LCMS1" "A small-footprint color management engine" "http://www.littlecms.com" TRUE "" "Needed by libkdcraw")
        # Flag to compile Little CMS version 1 with LibRaw
        ADD_DEFINITIONS(-DUSE_LCMS)
        # For compatibility
        SET(LCMS2_LIBRARIES ${LCMS_LIBRARIES})
    ELSE (LCMS_FOUND)
        MESSAGE(STATUS "Check for LCMS2 availability instead LCMS1...")
        FIND_PACKAGE(LCMS2)
        IF (LCMS2_FOUND AND (LCMS2_VERSION VERSION_EQUAL 2.1 OR LCMS2_VERSION VERSION_GREATER 2.1))
            MESSAGE(STATUS "Found LCMS2 instead LCMS1: ${LCMS2_LIBRARIES} ${LCMS2_INCLUDE_DIR}")
            INCLUDE_DIRECTORIES(${LCMS2_INCLUDE_DIR})
            MACRO_LOG_FEATURE(LCMS2_FOUND "LCMS2" "A small-footprint color management engine" "http://www.littlecms.com" FALSE "" "Needed by libkdcraw")
            # Flag to compile Little CMS version 2 with LibRaw
            ADD_DEFINITIONS(-DUSE_LCMS2)
        ENDIF (LCMS2_FOUND AND (LCMS2_VERSION VERSION_EQUAL 2.1 OR LCMS2_VERSION VERSION_GREATER 2.1))
    ENDIF (LCMS_FOUND)
ENDIF (ENABLE_LCMS2)

FIND_PACKAGE(JPEG)
IF(JPEG_FOUND)
    # check version of libjpeg
    # inspired from digikam/CMakeLists.txt
    SET(_jpeglib_version_source "#include <stddef.h>\n#include<stdio.h>\n#include <jpeglib.h>\n int main()\n {\n #if (JPEG_LIB_VERSION >= 80) \n #error JPEG_LIB_VERSION >= 80 found \n #endif \n }\n")
    SET(_jpeglib_version_source_file ${CMAKE_BINARY_DIR}/CMakeTmp/cmake_jpeglib_version_check.cpp)
    FILE(WRITE "${_jpeglib_version_source_file}" "${_jpeglib_version_source}")
    SET(_jpeglib_version_include_dirs "-DINCLUDE_DIRECTORIES:STRING=${JPEG_INCLUDE_DIR}")

    TRY_COMPILE(_jpeglib_version_compile_result ${CMAKE_BINARY_DIR} ${_jpeglib_version_source_file}
                CMAKE_FLAGS "${_jpeglib_version_include_dirs}"
                COMPILE_OUTPUT_VARIABLE _jpeglib_version_compile_output_var
               )
    IF(_jpeglib_version_compile_result)
        MESSAGE(STATUS "Identified libjpeg version less than 80 (too low)")
        SET(JPEG_FOUND FALSE)
    ENDIF()
ENDIF(JPEG_FOUND)

MACRO_LOG_FEATURE(JPEG_FOUND "libjpeg" "JPEG image format support" "http://www.ijg.org" FALSE "80" "needed for the LibRaw DNG lossy codec")

# To prevent warnings from M$ compiler
IF(WIN32 AND MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_ATL_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_AFX_SECURE_NO_WARNINGS)
ENDIF(WIN32 AND MSVC)

# OpenMP detection (requires CMake >= 2.6.3)
# NOTE: OpenMP under MacOSX do not support multithreading.

IF(NOT APPLE)
    MACRO_OPTIONAL_FIND_PACKAGE(OpenMP)

    IF(OPENMP_FOUND)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        IF("${OpenMP_CXX_FLAGS}" STREQUAL "-fopenmp")
            SET(OPENMP_LDFLAGS "-lgomp")
        ENDIF("${OpenMP_CXX_FLAGS}" STREQUAL "-fopenmp")
        IF("${OpenMP_CXX_FLAGS}" STREQUAL "-xopenmp")
            SET(OPENMP_LDFLAGS "-xopenmp")
        ENDIF("${OpenMP_CXX_FLAGS}" STREQUAL "-xopenmp")
    ENDIF(OPENMP_FOUND)
ENDIF(NOT APPLE)

# Flag to debug LibRaw
ADD_DEFINITIONS(-DDCRAW_VERBOSE)

# Flag to use libjpeg with LibRaw DNG lossy codec
if(JPEG_FOUND)
  ADD_DEFINITIONS(-DUSE_JPEG8)
endif()

# Flag to include demosaic pack GPL2
ADD_DEFINITIONS(-DLIBRAW_DEMOSAIC_PACK_GPL2)

# Flag to include demosaic pack GPL3
ADD_DEFINITIONS(-DLIBRAW_DEMOSAIC_PACK_GPL3)

# LibRaw use C++ exception
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

# Under Windows, use specific flag to compile.
IF(WIN32)
    ADD_DEFINITIONS(-DDJGPP)
ENDIF(WIN32)

IF(JPEG_FOUND)
    INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
ENDIF()

# For RawSpeed
FIND_PACKAGE(LibXml2)

IF(ENABLE_RAWSPEED AND JPEG_FOUND AND LIBXML2_FOUND)

    MESSAGE(STATUS "LibRaw will be compiled with RawSpeed codec")
    SET(RAWSPEED_SUPPORT_CAN_BE_COMPILED true)

ELSE()

    IF (NOT JPEG_FOUND)
        MESSAGE(STATUS "LibJPEG depency not resolved. LibRaw cannot be compiled with RawSpeed codec")
    ENDIF()

    IF (NOT LIBXML2_FOUND)
        MESSAGE(STATUS "LibXML2 depency not resolved. LibRaw cannot be compiled with RawSpeed codec")
    ENDIF()

    IF (NOT ENABLE_RAWSPEED)
        MESSAGE(STATUS "LibRaw will not be compiled with RawSpeed codec")
    ENDIF()

    SET(RAWSPEED_SUPPORT_CAN_BE_COMPILED false)

ENDIF()

# ==================================================================================================

IF(RAWSPEED_SUPPORT_CAN_BE_COMPILED)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed)

    INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})

    # Flag to include RawSpeed codec with Libraw
    ADD_DEFINITIONS(-DUSE_RAWSPEED)

    ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})

    # Do not compile RawSpeed with KDE FINAL mode.
    KDE4_NO_ENABLE_FINAL(librawspeed)

    SET(librawspeed_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/ArwDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/BitPumpJPEG.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/BitPumpMSB.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/BitPumpMSB32.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/BitPumpPlain.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/BlackArea.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/ByteStream.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/ByteStreamSwap.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/Camera.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/CameraMetaData.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/CameraMetadataException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/CameraSensorInfo.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/ColorFilterArray.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/Common.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/Cr2Decoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/DngDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/DngDecoderSlices.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/DngOpcodes.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/FileIOException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/FileMap.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/IOException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/LJpegDecompressor.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/LJpegPlain.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/NefDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/NikonDecompressor.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/OrfDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/PefDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/PentaxDecompressor.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/RawDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/RawDecoderException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/RawImage.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/RawImageDataFloat.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/RawImageDataU16.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/RawParser.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/Rw2Decoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/SrwDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/TiffEntry.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/TiffEntryBE.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/TiffIFD.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/TiffIFDBE.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/TiffParser.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/TiffParserException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/TiffParserHeaderless.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/libraw/RawSpeed/TiffParserOlympus.cpp
    )

    # Disable compilation warnings from RawSpeed. Just to be clear on the console.
    # Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (B.K.O: #269903)
    FOREACH(_curentfile ${librawspeed_LIB_SRCS})
        IF(WIN32 AND MSVC)
            SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w")
        ELSE(WIN32 AND MSVC)
            SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w -fPIC")
        ENDIF(WIN32 AND MSVC)
    ENDFOREACH(_curentfile ${librawspeed_LIB_SRCS})

    KDE4_ADD_LIBRARY(librawspeed STATIC ${librawspeed_LIB_SRCS})

    TARGET_LINK_LIBRARIES(librawspeed
                        ${MATH_LIBRARY}
                        ${JPEG_LIBRARY}
                        ${LIBXML2_LIBRARIES}
                        )

ENDIF()

# ==================================================================================================

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libraw
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/demosaic-pack-GPL2
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/demosaic-pack-GPL3
                   )

# Do not compile LibRaw with KDE FINAL mode.
KDE4_NO_ENABLE_FINAL(libraw)

SET(libraw_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/dcraw_common.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/dcraw_fileio.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/demosaic_packs.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_cxx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_c_api.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_datastream.cpp
   )

# Disable compilation warnings from LibRaw. Just to be clear on the console.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (B.K.O: #269903)
FOREACH(_curentfile ${libraw_LIB_SRCS})
    IF(WIN32 AND MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w")
    ELSE(WIN32 AND MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w -fPIC")
    ENDIF(WIN32 AND MSVC)
ENDFOREACH(_curentfile ${libraw_LIB_SRCS})

KDE4_ADD_LIBRARY(libraw STATIC ${libraw_LIB_SRCS})

TARGET_LINK_LIBRARIES(libraw
                      ${MATH_LIBRARY}
                      ${LCMS2_LIBRARIES}
                      ${OPENMP_LDFLAGS}
                     )
IF(JPEG_FOUND)
    TARGET_LINK_LIBRARIES(libraw ${JPEG_LIBRARY})
ENDIF()

SET_TARGET_PROPERTIES(libraw PROPERTIES COMPILE_FLAGS -DLIBRAW_BUILDLIB)

# =======================================================

IF(NOT WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake  ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.lsm)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
ENDIF(NOT WIN32)

ADD_SUBDIRECTORY(icons)
ADD_SUBDIRECTORY(libkdcraw)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(profiles)

MACRO_DISPLAY_FEATURE_LOG()
