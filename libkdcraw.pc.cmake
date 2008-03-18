prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=${BIN_INSTALL_DIR}
libdir=${LIB_INSTALL_DIR}
includedir=${INCLUDE_INSTALL_DIR}

Name: libkdcraw
Description: A C++ library for dcraw command line program to decode RAW pictures. This library is used by digiKam and kipi-plugins.
URL: http://www.kipi-plugins.org
Requires:
Version: ${DCRAW_LIB_VERSION_STRING}
Libs: -L${LIB_INSTALL_DIR} -lkdcraw
Cflags: -I${INCLUDE_INSTALL_DIR}
