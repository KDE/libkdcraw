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

KDE4_NO_ENABLE_FINAL(kdcraw)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkdcraw)

# add a small macro so that this is a bit cleaner
MACRO(LIBKDCRAW_BUILD_TEST)

    SET(_target ${ARGV0})
    SET(${_target}_SRCS ../libraw/samples/${_target}.cpp)
    SET_SOURCE_FILES_PROPERTIES(${${_target}_SRCS} PROPERTIES COMPILE_FLAGS -w)

    KDE4_ADD_EXECUTABLE(${_target} NOGUI ${${_target}_SRCS})

    TARGET_LINK_LIBRARIES(${_target} libraw ${CMAKE_THREAD_LIBS_INIT} ${OPENMP_LDFLAGS})

    IF(RAWSPEED_SUPPORT_CAN_BE_COMPILED)
        TARGET_LINK_LIBRARIES(${_target} librawspeed)
    ENDIF()

    IF(WIN32)
        TARGET_LINK_LIBRARIES(${_target} ws2_32)
    ENDIF(WIN32)

    SET_TARGET_PROPERTIES(${_target} PROPERTIES COMPILE_FLAGS -DLIBRAW_BUILDLIB)

ENDMACRO(LIBKDCRAW_BUILD_TEST)

SET(raw2png_SRCS raw2png.cpp)
KDE4_ADD_EXECUTABLE(raw2png NOGUI ${raw2png_SRCS})
TARGET_LINK_LIBRARIES(raw2png kdcraw ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OPENMP_LDFLAGS})

LIBKDCRAW_BUILD_TEST(simple_dcraw)
LIBKDCRAW_BUILD_TEST(mem_image)
LIBKDCRAW_BUILD_TEST(dcraw_emu)
LIBKDCRAW_BUILD_TEST(4channels)
LIBKDCRAW_BUILD_TEST(unprocessed_raw)
LIBKDCRAW_BUILD_TEST(raw-identify)
LIBKDCRAW_BUILD_TEST(multirender_test)
LIBKDCRAW_BUILD_TEST(postprocessing_benchmark)
#IF(WIN32)
#    LIBKDCRAW_BUILD_TEST(half_mt_win32)
#ELSE(WIN32)
#    LIBKDCRAW_BUILD_TEST(half_mt)
#    LIBKDCRAW_BUILD_TEST(dcraw_half)
#ENDIF(WIN32)
