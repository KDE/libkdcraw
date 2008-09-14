METASOURCES = AUTO

##################################################################

INCLUDES = -I../libraw $(all_includes)

# Requires by dcraw implementation from libraw.
KDE_OPTIONS = nofinal

# Requires by libraw.
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

noinst_LTLIBRARIES = libraw.la

libraw_la_CXXFLAGS = -DLIBAW_THREADS -DDCRAW_VERBOSE -DUSE_LCMS -w

libraw_la_SOURCES = ../libraw/src/libraw_cxx.cpp \
	                ../libraw/src/libraw_c_api.cpp \
	                ../libraw/internal/dcraw_common.cpp \
	                ../libraw/internal/foveon.cpp

libraw_la_LDFLAGS = $(LIBJPEG) $(LCMS_LIBS)

##################################################################

lib_LTLIBRARIES = libkdcraw.la

libkdcraw_la_SOURCES = kdcraw.cpp \
	                   kdcrawprivate.cpp \
	                   dcrawsettingswidget.cpp \
	                   rnuminput.cpp \
	                   rcombobox.cpp

libkdcraw_la_LDFLAGS  = $(all_libraries) $(KDE_RPATH) -version-info $(VERSION_INFO) -no-undefined
libkdcraw_la_LIBADD   = $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIB_KIO) libraw.la

libkdcrawinclude_HEADERS = rawdecodingsettings.h kdcraw.h dcrawsettingswidget.h \
                           rnuminput.h rcombobox.h dcrawinfocontainer.h rawfiles.h \
                           libkdcraw_export.h version.h 

libkdcrawincludedir = $(includedir)/libkdcraw
