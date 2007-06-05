#-- dcraw binary -------------------------------------------------

# libkdcraw use a dedicaced dcraw binary program to run. This is mandatory 
# since dcraw author have broken the command line options 
# compatibility with the 8.x serie.
# Also, dcraw is not available like a library.
# The dcraw.c source code is just a copy of official implementation, but 
# this one have been completly tested with libkdcraw core. 
# Note: dcraw project page is http://cybercom.net/~dcoffin

set(kdcraw_SRCS dcraw.c)
kde4_add_executable(kdcraw ${kdcraw_SRCS})
target_link_libraries(kdcraw jpeg lcms)

install(TARGETS kdcraw DESTINATION ${BIN_INSTALL_DIR})

#TODO : 
# - add install man page
# - do not compile dcraw.c with nofinal mode
# - add CFLAGS = "-Dinline= -w" to compile dcraw.c
