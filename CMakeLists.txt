PROJECT(libkdcraw)

# =======================================================
# Informations to update before to release this library.

# Library version history:
# API      ABI      
# 0.1.0 => 0.1.0    
# 0.1.1 => 1.0.0
# 0.1.2 => 2.0.0
# 0.1.3.=> 2.1.0
# 0.1.4 => 3.0.0
# ... here we can have new releases from KDE3 branch
# 0.2.0 => 5.0.0

# Library API version
SET(DCRAW_LIB_MAJOR_VERSION "0")
SET(DCRAW_LIB_MINOR_VERSION "2")
SET(DCRAW_LIB_PATCH_VERSION "0")

# Suffix to add at end of version string. Usual values are:
# "-svn"   : alpha code unstable from svn. Do not use in production
# "-beta1" : beta1 release.
# "-beta2" : beta2 release.
# "-beta3" : beta3 release.
# "-rc"    : release candidate.
# ""       : final relase. Can be used in production.
SET(DCRAW_LIB_SUFFIX_VERSION "-svn")

# Library ABI version used by linker.
# For details : http://www.gnu.org/software/libtool/manual.html#Updating-version-info
SET(DCRAW_LIB_SO_CUR_VERSION "5")
SET(DCRAW_LIB_SO_REV_VERSION "0")
SET(DCRAW_LIB_SO_AGE_VERSION "0")

# dcraw.c version include with this library. 
# Look into dcraw/dcraw.c implementation for details.
SET(DCRAW_PROGRAM_VERSION "8.84")

# =======================================================
# Set env. variables accordinly.

SET(DCRAW_PROGRAM_DIR "${LIB_INSTALL_DIR}/libkdcraw${DCRAW_LIB_SO_CUR_VERSION}")

SET(DCRAW_LIB_VERSION_STRING "${DCRAW_LIB_MAJOR_VERSION}.${DCRAW_LIB_MINOR_VERSION}.${DCRAW_LIB_PATCH_VERSION}${DCRAW_LIB_SUFFIX_VERSION}")
SET(DCRAW_LIB_VERSION_ID "0x0${DCRAW_LIB_MAJOR_VERSION}0${DCRAW_LIB_MINOR_VERSION}0${DCRAW_LIB_PATCH_VERSION}")

SET(DCRAW_LIB_SO_VERSION_STRING "${DCRAW_LIB_SO_CUR_VERSION}.${DCRAW_LIB_SO_REV_VERSION}.${DCRAW_LIB_SO_AGE_VERSION}")

# =======================================================

# Depencies detection require to compile dcraw.c
# Detect Little CMS library properlly with CMake script from 'cmake/modules'
FIND_PACKAGE(LCMS REQUIRED)

# Detect libJPEG
FIND_PACKAGE(JPEG REQUIRED)

# Detect libExpat use by DNG and XMP sdk
FIND_PACKAGE(EXPAT REQUIRED)

SUBDIRS(icons)
SUBDIRS(libkdcraw)
SUBDIRS(dcraw)
SUBDIRS(raw2dng)
SUBDIRS(profiles)

IF(NOT WIN32)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc)
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libkdcraw.lsm.cmake ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.lsm)
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
ENDIF(NOT WIN32)

INCLUDE_DIRECTORIES(${KDE4_INCLUDES} 
                    ${QT4_INCLUDES}
                    ${EXPAT_INCLUDE_DIR}
                    ./extra/md5
                    ./extra/dng_sdk
                    ./extra/xmp_sdk/XMPCore
                    ./extra/xmp_sdk/common
                    ./extra/xmp_sdk/include
                    ./extra/xmp_sdk/include/client-glue
           )

# Used by DNG SDK
ADD_DEFINITIONS(${KDE4_ENABLE_EXCEPTIONS}
                -DqWinOS=0
                -DqMacOS=0
                -DqDNGLittleEndian=1
                -DUNIX_ENV=1
               )

SET(libmd5_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/extra/md5/MD5.cpp)

SET(libdng_SRCS  
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_1d_function.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_date_time.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_ifd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_point.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_simple_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_1d_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_exceptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_memory_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_rational.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_spline.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xmp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_abort_sniffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_exif.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_image_writer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_mosaic_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_read_image.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xmp_sdk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_area_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_file_stream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_mutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_rect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_string.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_xy_coord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_bottlenecks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_filter_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_iptc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_negative.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_reference.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_string_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_camera_profile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_fingerprint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_linearization_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_orientation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_render.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tag_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_color_space.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_globals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_lossless_jpeg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_parse_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_resample.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_temperature.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_color_spec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_host.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_matrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_pixel_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_shared.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/dng_sdk/dng_tile_iterator.cpp
   )

SET(libxmp_SRCS  
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/ExpatAdapter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/ExpatAdapter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/ParseRDF.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/WXMPIterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/WXMPMeta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/WXMPUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMLParserAdapter.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPCore_Impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPCore_Impl.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPIterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPIterator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta-GetSet.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta-Parse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta-Serialize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPMeta.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPUtils-FileInfo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/XMPCore/XMPUtils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/common/UnicodeConversions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/common/UnicodeConversions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/TXMPFiles.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/TXMPIterator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/TXMPMeta.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/TXMPUtils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/XMP.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/XMP_BuildInfo.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/XMP_Const.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/XMP_Environment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/XMP_Version.h
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/client-glue/WXMPFiles.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/client-glue/WXMPIterator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/client-glue/WXMPMeta.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/client-glue/WXMPUtils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/extra/xmp_sdk/include/client-glue/WXMP_Common.hpp
   )
