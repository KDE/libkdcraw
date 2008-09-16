
METASOURCES = AUTO

INCLUDES = -I../libraw $(all_includes)

# LibRaw definitions
KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DDCRAW_VERBOSE -DUSE_LCMS

lib_LTLIBRARIES = libkdcraw.la

libkdcraw_la_SOURCES = kdcraw.cpp kdcrawprivate.cpp dcrawsettingswidget.cpp rnuminput.cpp rcombobox.cpp
libkdcraw_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -version-info $(VERSION_INFO) -no-undefined
libkdcraw_la_LIBADD = $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIB_KIO) ../libraw/libraw.la

libkdcrawinclude_HEADERS = rawdecodingsettings.h kdcraw.h dcrawsettingswidget.h \
                           rnuminput.h rcombobox.h dcrawinfocontainer.h rawfiles.h \
                           libkdcraw_export.h version.h 

libkdcrawincludedir = $(includedir)/libkdcraw
