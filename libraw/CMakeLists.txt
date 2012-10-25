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

IF(RAWSPEED_SUPPORT_CAN_BE_COMPILED)

    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RawSpeed)

    INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR} ${PTHREADS_INCLUDE_DIR})

    # Flag to include RawSpeed codec with Libraw
    ADD_DEFINITIONS(-DUSE_RAWSPEED)

    ADD_DEFINITIONS(${LIBXML2_DEFINITIONS} ${PTHREADS_DEFINITIONS})

    # Do not compile RawSpeed with KDE FINAL mode.
    KDE4_NO_ENABLE_FINAL(librawspeed)

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

    # Disable compilation warnings from RawSpeed. Just to be clear on the console.
    # Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (B.K.O: #269903)
    FOREACH(_curentfile ${librawspeed_LIB_SRCS})
        IF(WIN32 AND MSVC)
            SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w")
        ELSE(WIN32 AND MSVC)
            SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w -fPIC")
        ENDIF(WIN32 AND MSVC)
    ENDFOREACH(_curentfile ${librawspeed_LIB_SRCS})

    ADD_LIBRARY(librawspeed STATIC ${librawspeed_LIB_SRCS})

    TARGET_LINK_LIBRARIES(librawspeed
                          ${MATH_LIBRARY}
                          ${JPEG_LIBRARY}
                          ${LIBXML2_LIBRARIES}
                          ${PTHREADS_LIBRARY}
                         )

ENDIF()

# =================================================================================================

# Flag to include demosaic pack GPL2
ADD_DEFINITIONS(-DLIBRAW_DEMOSAIC_PACK_GPL2)

# Flag to include demosaic pack GPL3
ADD_DEFINITIONS(-DLIBRAW_DEMOSAIC_PACK_GPL3)

# Flag to debug LibRaw
ADD_DEFINITIONS(-DDCRAW_VERBOSE)

# Flag used by default into LibRaw to not use dedicated external thread
ADD_DEFINITIONS(-DLIBRAW_NOTHREADS)

IF (WIN32)
    ADD_DEFINTION(-DLIBRAW_BUILDLIB)
ENDIF()

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

# Disable compilation warnings from LibRaw. Just to be clear on the console.
# Adjust flag for static lib and 64 bits computers using -fPIC for GCC compiler (B.K.O: #269903)
FOREACH(_curentfile ${libraw_LIB_SRCS})
    IF(WIN32 AND MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w")
    ELSE(WIN32 AND MSVC)
        SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS "-w -fPIC")
    ENDIF(WIN32 AND MSVC)
ENDFOREACH(_curentfile ${libraw_LIB_SRCS})

ADD_LIBRARY(libraw STATIC ${libraw_LIB_SRCS})

SET_TARGET_PROPERTIES(libraw PROPERTIES COMPILE_FLAGS -DLIBRAW_BUILDLIB)

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

# =================================================================================================

# add a small macro so that this is a bit cleaner
MACRO(LIBRAW_BUILD_SAMPLES)

    SET(_filename ${ARGV0})
    STRING(REPLACE "." ";" _temp ${_filename})
    LIST(GET _temp 0 _target)

    SET(${_target}_SRCS samples/${_filename})
    SET_SOURCE_FILES_PROPERTIES(${${_target}_SRCS} PROPERTIES COMPILE_FLAGS -w)

    ADD_EXECUTABLE(${_target} ${${_target}_SRCS})

    TARGET_LINK_LIBRARIES(${_target} libraw ${OPENMP_LDFLAGS})

    IF(RAWSPEED_SUPPORT_CAN_BE_COMPILED)
        TARGET_LINK_LIBRARIES(${_target} librawspeed)
    ENDIF()

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
#IF(WIN32)
#    LIBRAW_BUILD_SAMPLES(half_mt_win32.c)
#ELSE(WIN32)
#    LIBRAW_BUILD_SAMPLES(dcraw_half.c)
#    LIBRAW_BUILD_SAMPLES(half_mt.c)
#ENDIF(WIN32)
