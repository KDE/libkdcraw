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

# General definitions rules

# To prevent warnings from M$ compiler
IF(WIN32 AND MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_ATL_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_AFX_SECURE_NO_WARNINGS)
ENDIF(WIN32 AND MSVC)

# Under Windows, use specific flag to compile.
IF(WIN32)
    ADD_DEFINITIONS(-DDJGPP)
ENDIF(WIN32)

# LibRaw use C++ exception${CMAKE_THREAD_LIBS_INIT}
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

# -- Check dependencies --------------------------------------------------------------------------------

# Math library check

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

# JPEG library check

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
               )
    IF(_jpeglib_version_compile_result)
        MESSAGE(STATUS "Identified libjpeg version less than 80 (too low)")
        SET(JPEG_FOUND FALSE)
    ENDIF()
ENDIF(JPEG_FOUND)

MACRO_LOG_FEATURE(JPEG_FOUND "libjpeg" "JPEG image format support" "http://www.ijg.org" FALSE "80" "needed for the LibRaw DNG lossy codec")

# Flag to use libjpeg with LibRaw DNG lossy codec
IF(JPEG_FOUND)
    ADD_DEFINITIONS(-DUSE_JPEG)
    ADD_DEFINITIONS(-DUSE_JPEG8)
    INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
ENDIF()

# OpenMP library detection (requires CMake >= 2.6.3)
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

MACRO_BOOL_TO_01(OPENMP_FOUND LIBRAW_USE_OPENMP)

# Jasper library check

FIND_PACKAGE(Jasper)

# Flag to use libjasper with LibRaw RedCine codec
IF(JASPER_FOUND)
    ADD_DEFINITIONS(-DUSE_JASPER)
    INCLUDE_DIRECTORIES(${JASPER_INCLUDE_DIR})
ENDIF()

# For RawSpeed Codec Support
FIND_PACKAGE(LibXml2)
FIND_PACKAGE(Pthreads)

IF(ENABLE_RAWSPEED AND JPEG_FOUND AND LIBXML2_FOUND AND PTHREADS_FOUND)

    MESSAGE(STATUS "LibRaw will be compiled with RawSpeed codec")
    SET(RAWSPEED_SUPPORT_CAN_BE_COMPILED true)

ELSE()

    IF (NOT JPEG_FOUND)
        MESSAGE(STATUS "LibJPEG depency not resolved. LibRaw cannot be compiled with RawSpeed codec")
    ENDIF()

    IF (NOT LIBXML2_FOUND)
        MESSAGE(STATUS "LibXML2 depency not resolved. LibRaw cannot be compiled with RawSpeed codec")
    ENDIF()

    IF (NOT PTHREADS_FOUND)
        MESSAGE(STATUS "Pthreads depency not resolved. LibRaw cannot be compiled with RawSpeed codec")
    ENDIF()

    IF (NOT ENABLE_RAWSPEED)
        MESSAGE(STATUS "LibRaw will not be compiled with RawSpeed codec")
    ENDIF()

    SET(RAWSPEED_SUPPORT_CAN_BE_COMPILED false)

ENDIF()

# -- Compile RAWSpeed library --------------------------------------------------------------------------------

IF(RAWSPEED_SUPPORT_CAN_BE_COMPILED)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed)

    INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${PTHREADS_INCLUDE_DIR})

    # Flag to include RawSpeed codec with Libraw
    ADD_DEFINITIONS(-DUSE_RAWSPEED)

    ADD_DEFINITIONS(${LIBXML2_DEFINITIONS} ${PTHREADS_DEFINITIONS})

    SET(librawspeed_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/ArwDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/BitPumpJPEG.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/BitPumpMSB.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/BitPumpMSB32.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/BitPumpPlain.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/BlackArea.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/ByteStream.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/ByteStreamSwap.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/Camera.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/CameraMetaData.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/CameraMetadataException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/CameraSensorInfo.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/ColorFilterArray.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/Common.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/Cr2Decoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/DngDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/DngDecoderSlices.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/DngOpcodes.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/FileIOException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/FileMap.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/IOException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/LJpegDecompressor.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/LJpegPlain.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/NefDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/NikonDecompressor.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/OrfDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/PefDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/PentaxDecompressor.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/RawDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/RawDecoderException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/RawImage.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/RawImageDataFloat.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/RawImageDataU16.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/RawParser.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/Rw2Decoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/SrwDecoder.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/TiffEntry.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/TiffEntryBE.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/TiffIFD.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/TiffIFDBE.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/TiffParser.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/TiffParserException.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/TiffParserHeaderless.cpp
                             ${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed/TiffParserOlympus.cpp
    )

ENDIF()

MACRO_BOOL_TO_01(RAWSPEED_SUPPORT_CAN_BE_COMPILED LIBRAW_USE_RAWSPEED)

# -- Compile LibRaw library --------------------------------------------------------------------------------

# Flag to include demosaic pack GPL2
ADD_DEFINITIONS(-DLIBRAW_DEMOSAIC_PACK_GPL2)

# Flag to include demosaic pack GPL3
ADD_DEFINITIONS(-DLIBRAW_DEMOSAIC_PACK_GPL3)

# Flag to debug LibRaw
ADD_DEFINITIONS(-DDCRAW_VERBOSE)

# Flag used by default into LibRaw to not use dedicated external thread
ADD_DEFINITIONS(-DLIBRAW_NOTHREADS)

# Flag to export library symbols
IF (WIN32)
    ADD_DEFINITIONS(-DLIBRAW_BUILDLIB)
ENDIF()

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config-libraw.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-libraw.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/
                    ${CMAKE_CURRENT_SOURCE_DIR}/demosaic-pack-GPL2
                    ${CMAKE_CURRENT_SOURCE_DIR}/demosaic-pack-GPL3
                   )

# Do not compile LibRaw with KDE FINAL mode.
KDE4_NO_ENABLE_FINAL(libraw)

SET(libraw_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/internal/dcraw_common.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/internal/dcraw_fileio.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/internal/demosaic_packs.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/libraw_cxx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/libraw_c_api.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/src/libraw_datastream.cpp
   )

IF(RAWSPEED_SUPPORT_CAN_BE_COMPILED)
    SET(libraw_LIB_SRCS ${libraw_LIB_SRCS} ${librawspeed_LIB_SRCS})
ENDIF()

# Disable compilation warnings from LibRaw. Just to be clear on the console.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (B.K.O: #269903)
# Use O4 GCC compilation option to prevent artifacts with OpenMP (B.K.O #305382)
FOREACH(_curentfile ${libraw_LIB_SRCS})
    IF(WIN32 AND MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w")
    ELSE(WIN32 AND MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w -fPIC -O4")
    ENDIF(WIN32 AND MSVC)
ENDFOREACH(_curentfile ${libraw_LIB_SRCS})

ADD_LIBRARY(libraw STATIC ${libraw_LIB_SRCS})

TARGET_LINK_LIBRARIES(libraw
                      ${MATH_LIBRARY}
                      ${LCMS2_LIBRARIES}
                      ${OPENMP_LDFLAGS}
                     )
IF(JPEG_FOUND)
    TARGET_LINK_LIBRARIES(libraw ${JPEG_LIBRARY})
ENDIF()

IF(JASPER_FOUND)
    TARGET_LINK_LIBRARIES(libraw ${JASPER_LIBRARIES})
ENDIF()

IF(RAWSPEED_SUPPORT_CAN_BE_COMPILED)
    TARGET_LINK_LIBRARIES(libraw ${LIBXML2_LIBRARIES} ${PTHREADS_LIBRARY})
ENDIF()

# -- Compile LibRaw Samples --------------------------------------------------------------------------------

# add a small macro so that this is a bit cleaner
MACRO(LIBRAW_BUILD_SAMPLES)

    SET(_filename ${ARGV0})
    STRING(REPLACE "." ";" _temp ${_filename})
    LIST(GET _temp 0 _target)

    SET(${_target}_SRCS samples/${_filename})
    SET_SOURCE_FILES_PROPERTIES(${${_target}_SRCS} PROPERTIES COMPILE_FLAGS -w)

    ADD_EXECUTABLE(${_target} ${${_target}_SRCS})

    TARGET_LINK_LIBRARIES(${_target} libraw ${OPENMP_LDFLAGS})

    IF(WIN32)
        TARGET_LINK_LIBRARIES(${_target} ws2_32)
    ENDIF(WIN32)

ENDMACRO(LIBRAW_BUILD_SAMPLES)

LIBRAW_BUILD_SAMPLES(simple_dcraw.cpp)
LIBRAW_BUILD_SAMPLES(mem_image.cpp)
LIBRAW_BUILD_SAMPLES(dcraw_emu.cpp)
LIBRAW_BUILD_SAMPLES(4channels.cpp)
LIBRAW_BUILD_SAMPLES(unprocessed_raw.cpp)
LIBRAW_BUILD_SAMPLES(raw-identify.cpp)
LIBRAW_BUILD_SAMPLES(multirender_test.cpp)
LIBRAW_BUILD_SAMPLES(postprocessing_benchmark.cpp)

# C sample files will be compiled only if LCMS v2 is detected, to prevent broken compilation
IF(LCMS2_FOUND)
   IF(WIN32)
        LIBRAW_BUILD_SAMPLES(half_mt_win32.c)
   ELSE(WIN32)
        LIBRAW_BUILD_SAMPLES(dcraw_half.c)
        LIBRAW_BUILD_SAMPLES(half_mt.c)
   ENDIF(WIN32)
ENDIF(LCMS2_FOUND)
