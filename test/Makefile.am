KDE_OPTIONS = nofinal
METASOURCES = AUTO

INCLUDES = -I../libraw -I../libkdcraw $(all_includes)

bin_PROGRAMS = raw2png

raw2png_CFLAGS = $(all_includes)

raw2png_SOURCES = raw2png.cpp

raw2png_LDFLAGS = $(all_libraries) ../libkdcraw/libkdcraw.la
