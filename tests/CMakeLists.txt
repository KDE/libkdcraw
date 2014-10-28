# ===========================================================
#
# This file is a part of digiKam project
# <a href="http://www.digikam.org">http://www.digikam.org</a>
#
# @date   2006-12-09
# @brief  a tread-safe libraw C++ program interface for KDE
#
# @author Copyright (C) 2006-2014 by Gilles Caulier
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

# TODO
#add_subdirectory(multithreading)

set(raw2png_SRCS raw2png.cpp)
add_executable(raw2png ${raw2png_SRCS})
target_link_libraries(raw2png KF5KDcraw)

set(libinfo_SRCS libinfo.cpp)
add_executable(libinfo ${libinfo_SRCS})
target_link_libraries(libinfo KF5KDcraw)
