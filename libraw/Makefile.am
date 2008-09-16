
# LibRaw definitions
KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DLIBRAW_THREADS -DDCRAW_VERBOSE -DUSE_LCMS

INCLUDES = $(all_includes)

# Requires by dcraw implementation from libraw.
KDE_OPTIONS = nofinal

noinst_LTLIBRARIES = libraw.la

libraw_la_CXXFLAGS = -w
libraw_la_SOURCES = src/libraw_cxx.cpp src/libraw_c_api.cpp internal/dcraw_common.cpp internal/foveon.cpp
libraw_la_LDFLAGS = $(LIBJPEG) $(LCMS_LIBS)
