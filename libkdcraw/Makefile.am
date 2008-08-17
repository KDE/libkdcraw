METASOURCES = AUTO

KDE_CXXFLAGS = -DKDCRAW_DIR='"$(KDCRAW_DIR)"'

INCLUDES= $(all_includes)

lib_LTLIBRARIES = libkdcraw.la

libkdcraw_la_SOURCES = dcrawbinary.cpp kdcraw.cpp dcrawsettingswidget.cpp rnuminput.cpp rcombobox.cpp 

libkdcraw_la_LDFLAGS  = $(all_libraries) $(KDE_RPATH) -version-info $(VERSION_INFO) -no-undefined
libkdcraw_la_LIBADD   = $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT)

libkdcrawinclude_HEADERS = rawdecodingsettings.h kdcraw.h dcrawbinary.h dcrawsettingswidget.h \
                           rnuminput.h rcombobox.h dcrawinfocontainer.h rawfiles.h \
                           libkdcraw_export.h version.h 

libkdcrawincludedir = $(includedir)/libkdcraw

