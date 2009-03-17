KDE_OPTIONS = nofinal
METASOURCES = AUTO

# LibRaw definitions
KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DDCRAW_VERBOSE -DUSE_LCMS

INCLUDES = -I../libraw -I../libkdcraw $(all_includes)

check_PROGRAMS = raw2png identify simple_dcraw mem_image dcraw_emu 4channels unprocessed_raw

# -------------------------------------------------------------

raw2png_CFLAGS = $(all_includes)
raw2png_SOURCES = raw2png.cpp
raw2png_LDFLAGS = $(all_libraries) ../libkdcraw/libkdcraw.la

# -------------------------------------------------------------

identify_CFLAGS = $(all_includes)
identify_SOURCES = ../libraw/samples/identify.cpp
identify_LDFLAGS = $(all_libraries) ../libraw/libraw.la

# -------------------------------------------------------------

simple_dcraw_CFLAGS = $(all_includes)
simple_dcraw_SOURCES = ../libraw/samples/simple_dcraw.cpp
simple_dcraw_LDFLAGS = $(all_libraries) ../libraw/libraw.la

# -------------------------------------------------------------

mem_image_CFLAGS = $(all_includes)
mem_image_SOURCES = ../libraw/samples/mem_image.cpp
mem_image_LDFLAGS = $(all_libraries) ../libraw/libraw.la

# -------------------------------------------------------------

dcraw_emu_CFLAGS = $(all_includes)
dcraw_emu_SOURCES = ../libraw/samples/dcraw_emu.cpp
dcraw_emu_LDFLAGS = $(all_libraries) ../libraw/libraw.la

# -------------------------------------------------------------

4channels_CFLAGS = $(all_includes)
4channels_SOURCES = ../libraw/samples/4channels.cpp
4channels_LDFLAGS = $(all_libraries) ../libraw/libraw.la

# -------------------------------------------------------------

unprocessed_raw_CFLAGS = $(all_includes)
unprocessed_raw_SOURCES = ../libraw/samples/unprocessed_raw.cpp
unprocessed_raw_LDFLAGS = $(all_libraries) ../libraw/libraw.la
