#
# SPDX-FileCopyrightText: 2010-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(raw2png_SRCS raw2png.cpp)
add_executable(raw2png ${raw2png_SRCS})
target_link_libraries(raw2png KF5KDcraw)

set(libinfo_SRCS libinfo.cpp)
add_executable(libinfo ${libinfo_SRCS})
target_link_libraries(libinfo KF5KDcraw)
