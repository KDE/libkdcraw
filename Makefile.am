SUBDIRS = dcraw 
METASOURCES = AUTO

INCLUDES= $(all_includes)

lib_LTLIBRARIES = libkdcraw.la

libkdcraw_la_SOURCES = dcrawbinary.cpp kdcraw.cpp dcrawsettingswidget.cpp

libkdcraw_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info 0:1:0 -no-undefined
libkdcraw_la_LIBADD  = $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

libkdcrawinclude_HEADERS = rawdecodingsettings.h kdcraw.h dcrawbinary.h dcrawsettingswidget.h \
                           dcrawinfocontainer.h rawfiles.h libkdcraw_export.h version.h 

libkdcrawincludedir = $(includedir)/libkdcraw

libkdcrawpkgconfigdir = $(libdir)/pkgconfig
libkdcrawpkgconfig_DATA = libkdcraw.pc
EXTRA_DIST = libkdcraw.pc.in

# i18n translation messages
messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
		$(XGETTEXT) $$LIST -o $(podir)/libkdcraw.pot; \
	fi