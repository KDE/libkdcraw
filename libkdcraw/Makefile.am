METASOURCES = AUTO

INCLUDES= $(all_includes)

lib_LTLIBRARIES = libkdcraw.la

libkdcraw_la_SOURCES = dcrawbinary.cpp kdcraw.cpp dcrawsettingswidget.cpp

libkdcraw_version_info = 0:1:0
libkdcraw_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info $(libkdcraw_version_info) -no-undefined
libkdcraw_la_LIBADD  = $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

libkdcrawinclude_HEADERS = rawdecodingsettings.h kdcraw.h dcrawbinary.h dcrawsettingswidget.h \
                           dcrawinfocontainer.h rawfiles.h libkdcraw_export.h version.h 

libkdcrawincludedir = $(includedir)/libkdcraw

