SUBDIRS = dcraw 
METASOURCES = AUTO

INCLUDES= $(all_includes)

lib_LTLIBRARIES = libkdcraw.la

libkdcraw_la_SOURCES = dcrawbinary.cpp dcrawiface.cpp

libkdcraw_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info 0:1:0 -no-undefined
libkdcraw_la_LIBADD  = $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

libkdcrawinclude_HEADERS = rawdecodingsettings.h dcrawiface.h dcrawbinary.h libkdcraw_export.h version.h 

libkdcrawincludedir = $(includedir)/libkdcraw

libkdcrawpkgconfigdir = $(libdir)/pkgconfig
libkdcrawpkgconfig_DATA = libkdcraw.pc
EXTRA_DIST = libkdcraw.pc.in

