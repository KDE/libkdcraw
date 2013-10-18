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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkdcraw)

SET(raw2png_SRCS raw2png.cpp)
ADD_EXECUTABLE(raw2png ${raw2png_SRCS})
TARGET_LINK_LIBRARIES(raw2png kdcraw ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

SET(libinfo_SRCS libinfo.cpp)
ADD_EXECUTABLE(libinfo ${libinfo_SRCS})
TARGET_LINK_LIBRARIES(libinfo kdcraw ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
