PROJECT(libkdcraw)

FIND_PACKAGE(KDE4 REQUIRED)
INCLUDE(KDE4Defaults)
INCLUDE(MacroLibrary)
INCLUDE(MacroOptionalAddSubdirectory)
INCLUDE(FindPackageHandleStandardArgs)

ADD_DEFINITIONS (${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
INCLUDE_DIRECTORIES (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

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
# 1.0.0 => 8.0.0  (Released with KDE 4.4.0

# Library API version
SET(DCRAW_LIB_MAJOR_VERSION "1")
SET(DCRAW_LIB_MINOR_VERSION "0")
SET(DCRAW_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(DCRAW_LIB_SUFFIX_VERSION "")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual/libtool.html#Updating-version-info
SET(DCRAW_LIB_SO_CUR_VERSION "8")
SET(DCRAW_LIB_SO_REV_VERSION "0")
SET(DCRAW_LIB_SO_AGE_VERSION "0")

# ==================================================================================================
# Set env. variables accordinly.

SET(DCRAW_LIB_VERSION_STRING "${DCRAW_LIB_MAJOR_VERSION}.${DCRAW_LIB_MINOR_VERSION}.${DCRAW_LIB_PATCH_VERSION}${DCRAW_LIB_SUFFIX_VERSION}")
SET(DCRAW_LIB_VERSION_ID "0x0${DCRAW_LIB_MAJOR_VERSION}0${DCRAW_LIB_MINOR_VERSION}0${DCRAW_LIB_PATCH_VERSION}")

SET(DCRAW_LIB_SO_VERSION_STRING "${DCRAW_LIB_SO_CUR_VERSION}.${DCRAW_LIB_SO_REV_VERSION}.${DCRAW_LIB_SO_AGE_VERSION}")

# ==================================================================================================
# LibRAW source code

IF(NOT WIN32)
    FIND_LIBRARY(MATH_LIBRARY m)
ENDIF(NOT WIN32)

FIND_PACKAGE(LCMS REQUIRED)

# Flag to compile Little CMS code from LibRaw
ADD_DEFINITIONS(-DUSE_LCMS)

# OpenMP detection requires CMake >= 2.6.3

EXEC_PROGRAM(${CMAKE_COMMAND} ARGS --version OUTPUT_VARIABLE VERSION_CMAKE_INFO)
MESSAGE(STATUS "CMake version: ${VERSION_CMAKE_INFO}")
STRING(REPLACE "-patch " "." VERSION_CMAKE_INFO "${VERSION_CMAKE_INFO}")
MESSAGE(STATUS "CMake version (cleaned): ${VERSION_CMAKE_INFO}")
MACRO_ENSURE_VERSION("2.6.3" "${VERSION_CMAKE_INFO}" VERSION_CMAKE)

IF(VERSION_CMAKE)
    MACRO_OPTIONAL_FIND_PACKAGE(OpenMP)
    IF(OPENMP_FOUND)
        MESSAGE(STATUS "OpenMP detected: LibRaw will be compiled with parallelized code.")
        SET(USE_OPENMP TRUE)
    ELSEIF(OPENMP_FOUND)
        MESSAGE(STATUS "OpenMP not detected: LibRaw will not be compiled with parallelized code.")
        SET(USE_OPENMP FALSE)
    ENDIF(OPENMP_FOUND)
ELSE(VERSION_CMAKE)
    MESSAGE(STATUS "CMake version too old: cannot detected OpenMP: LibRaw will not be compiled with parallelized code.")
    SET(USE_OPENMP FALSE)
ENDIF(VERSION_CMAKE)

IF(OPENMP_FOUND)
    ADD_DEFINITIONS(${OpenMP_CXX_FLAGS}) 
    IF(CMAKE_COMPILER_IS_GNUCXX)
        SET(OPENMP_LDFLAGS "-lgomp")
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF(OPENMP_FOUND)

# Flag to debug LibRaw
ADD_DEFINITIONS(-DDCRAW_VERBOSE)

# LibRaw use C++ exception.
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

# Under Windows, use specific flag to compile.
IF(WIN32)
    ADD_DEFINITIONS( -DDJGPP )
ENDIF(WIN32)

# ==================================================================================================

INCLUDE_DIRECTORIES(
                    ${JPEG_INCLUDE_DIR}
                    ${LCMS_INCLUDE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw
                   )

# Do not compile LibRaw with KDE FINAL mode.
KDE4_NO_ENABLE_FINAL(libraw)

SET(libraw_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/dcraw_common.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/dcraw_fileio.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/internal/foveon.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_cxx.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/libraw/src/libraw_c_api.cpp
   )

# =======================================================

IF(NOT WIN32)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.lsm)
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
ENDIF(NOT WIN32)

SUBDIRS(icons)
SUBDIRS(libkdcraw)
SUBDIRS(test)
SUBDIRS(profiles)
