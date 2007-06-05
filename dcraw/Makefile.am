# Requires by dcraw.c implementation.
KDE_OPTIONS = nofinal

METASOURCES = AUTO

#-- dcraw binary -------------------------------------------------

# libkdcraw use a dedicaced dcraw binary program to run. This is mandatory 
# since dcraw author have broken the command line options 
# compatibility with the 8.x serie.
# Also, dcraw is not available like a library.
# The dcraw.c source code is just a copy of official implementation, but 
# this one have been completly tested with libkdcraw core. 
# Note: dcraw project page is http://cybercom.net/~dcoffin

bin_PROGRAMS = kdcraw

# The manual page of binary program.
man_MANS = kdcraw.1

# This line is require to prevent broken compilation with -std=iso9899:1990 gcc option.
# In fact, dcraw.c use lcms.h witch use the "inline" keyword witch do not exist in pure C 
# implementation. The -Dinline option please compiler to see inline with this option.
# Note : using -std-c99 instead -std=iso9899:1990 is not possible because some compiler 
# do not support it.
# Thanks to Andras Mantia for this tip.
kdcraw_CFLAGS = -Dinline= -w

kdcraw_SOURCES = dcraw.c

kdcraw_LDFLAGS = $(all_libraries) $(LIBJPEG) $(LCMS_LIBS) 


