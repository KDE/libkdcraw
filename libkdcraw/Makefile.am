METASOURCES = AUTO

INCLUDES= $(all_includes)

lib_LTLIBRARIES = libkdcraw.la

libkdcraw_la_SOURCES = dcrawbinary.cpp kdcraw.cpp dcrawsettingswidget.cpp

libkdcraw_la_LDFLAGS  = $(all_libraries) $(KDE_RPATH) -version-info $(VERSION_INFO) -no-undefined
libkdcraw_la_LIBADD   = $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)
libkdcraw_la_CXXFLAGS = -DKDCRAW_DIR='"$(KDCRAW_DIR)"'

libkdcrawinclude_HEADERS = rawdecodingsettings.h kdcraw.h dcrawbinary.h dcrawsettingswidget.h \
                           dcrawinfocontainer.h rawfiles.h libkdcraw_export.h version.h 

libkdcrawincludedir = $(includedir)/libkdcraw

