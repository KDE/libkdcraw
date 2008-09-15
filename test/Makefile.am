KDE_OPTIONS = nofinal
METASOURCES = AUTO

# Requires by libraw.
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

INCLUDES = -I../libraw -I../libkdcraw $(all_includes)

bin_PROGRAMS = raw2png identify

# -------------------------------------------------------------

raw2png_CFLAGS = $(all_includes)
raw2png_SOURCES = raw2png.cpp
raw2png_LDFLAGS = $(all_libraries) ../libkdcraw/libkdcraw.la

# -------------------------------------------------------------

identify_CFLAGS = $(all_includes)
identify_SOURCES = ../libraw/samples/identify.cpp
identify_LDFLAGS = $(all_libraries) ../libraw/libraw.la
