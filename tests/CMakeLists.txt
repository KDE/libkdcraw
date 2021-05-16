#
# SPDX-FileCopyrightText: 2010-2015 Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_executable(raw2png)
target_sources(raw2png PRIVATE raw2png.cpp)
target_link_libraries(raw2png KDcraw)

add_executable(libinfo)
target_sources(libinfo PRIVATE libinfo.cpp)
target_link_libraries(libinfo KDcraw)
