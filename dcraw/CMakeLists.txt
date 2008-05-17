#-- dcraw binary for KDE -------------------------------------------------

# libkdcraw use a dedicaced dcraw binary program to run. This is mandatory 
# since dcraw author have broken the command line options 
# compatibility with the 8.x serie.
# Also, dcraw is not available like a library.
# The dcraw.c source code is just a copy of official implementation, but 
# this one have been completly tested with libkdcraw core. 
# Note: dcraw project page is http://cybercom.net/~dcoffin

# Require by dcraw.c implementation.
KDE4_NO_ENABLE_FINAL(kdcraw)
INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR} ${LCMS_INCLUDE_DIR})

# This line is required to prevent broken compilation with the
# -std=iso9899:1990 gcc option.
# In fact, dcraw.c use lcms.h which use the "inline" keyword, which do not
# exist in pure C implementations. The -Dinline= option simply defines inline
# to nothing at all.
# 
# Note : using -std-c99 instead -std=iso9899:1990 is not possible because some
# compilers do not support it.
# Thanks to Andras Mantia for this tip.
ADD_DEFINITIONS("-Dinline= -w")

SET(kdcraw_SRCS dcraw.c)
KDE4_ADD_EXECUTABLE(kdcraw_bin ${kdcraw_SRCS})
TARGET_LINK_LIBRARIES(kdcraw_bin ${JPEG_LIBRARY} ${LCMS_LIBRARIES})

# the target kdcraw binary programe name add the soname of library as suffixe.
SET_TARGET_PROPERTIES(kdcraw_bin PROPERTIES OUTPUT_NAME kdcraw)

# Note: we don't install kcraw man page. It will never used by users.
# The kdcraw program is installled in install dir of library to able 
# to install more than one version at the same time on the same computer.
INSTALL(TARGETS kdcraw_bin DESTINATION ${DCRAW_PROGRAM_DIR})

# Install Camera list supported by dcraw.
INSTALL(FILES CAMERALIST DESTINATION ${DCRAW_PROGRAM_DIR})

