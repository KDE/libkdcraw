PROJECT(libkdcraw)

OPTION(DISABLE_OPENMP "Build without LibRaw OpenMP support" OFF)

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

# Library API version
SET(DCRAW_LIB_MAJOR_VERSION "0")
SET(DCRAW_LIB_MINOR_VERSION "5")
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
# For details : http://www.gnu.org/software/libtool/manual.html#Updating-version-info
SET(DCRAW_LIB_SO_CUR_VERSION "7")
SET(DCRAW_LIB_SO_REV_VERSION "1")
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

find_package(LCMS REQUIRED)

# Flag to compile Little CMS code from LibRaw
ADD_DEFINITIONS(-DUSE_LCMS)

# Flag to debug LibRaw
ADD_DEFINITIONS(-DDCRAW_VERBOSE)

# LibRaw use C++ exception.
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS})

# Under Windows, use specific flag to compile.
IF( WIN32 )
    ADD_DEFINITIONS( -DDJGPP )
ENDIF( WIN32 )

# ==================================================================================================
# get the gcc version                                                                               

# Check gcc version to see if OpenMP support is available to perform parallelized
# demosaicing with LibRaw

IF(NOT DISABLE_OPENMP)

    IF(CMAKE_COMPILER_IS_GNUCXX)

        EXEC_PROGRAM(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE VERSION_GCC_INFO)
        STRING(REGEX MATCH " [34]\\.[0-9]\\.[0-9]" GCC_VERSION "${VERSION_GCC_INFO}")      

        # gcc on mac just reports: "gcc (GCC) 3.3 20030304 ..." without the patch level, handle this here:
        IF(NOT GCC_VERSION)
            STRING(REGEX REPLACE ".*\\(GCC\\).* ([34]\\.[0-9]) .*" "\\1.0" GCC_VERSION "${VERSION_GCC_INFO}")
        ENDIF (NOT GCC_VERSION)

        #Only GCC >= 4.3.2 support properly OpenMP, especially with KIOSlave.
        MACRO_ENSURE_VERSION("4.3.2" "${GCC_VERSION}" GCC_VERSION_GOOD)

        IF(GCC_VERSION_GOOD)

            MESSAGE(STATUS "GCC version (${GCC_VERSION} ) detected: we will use OpenMP extension with LibRaw.")
            ADD_DEFINITIONS(-fopenmp)
            SET(OPENMP_LDFLAGS -lgomp)
	    SET(USE_OPENMP TRUE)

        ELSEIF(GCC_VERSION_GOOD)

            MESSAGE(STATUS "Compiler is an older GCC. OpenMP extension support with LibRaw is disabled.")
            SET(USE_OPENMP FALSE)
	
        ENDIF(GCC_VERSION_GOOD)

    ELSEIF(CMAKE_COMPILER_IS_GNUCXX)

        MESSAGE(STATUS "Compiler is not GCC. OpenMP extension support with LibRaw is disabled.")
        SET(USE_OPENMP FALSE)
	
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ELSEIF(NOT DISABLE_OPENMP)

    MESSAGE(STATUS "OpenMP extension support with LibRaw is disabled.")
    SET(USE_OPENMP FALSE)

ENDIF(NOT DISABLE_OPENMP)

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
