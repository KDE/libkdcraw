
KDE4_NO_ENABLE_FINAL(kdcraw)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkdcraw)

# Disable dcraw C Ansi code warnings from LibRaw. Just to be clear on the console.
FOREACH(_curentfile ${libraw_LIB_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w)
ENDFOREACH(_curentfile ${libraw_LIB_SRCS})

# add a small macro so that this is a bit cleaner
macro(libkdcraw_build_test)
    SET(_target ${ARGV0})
    SET(${_target}_SRCS ../libraw/samples/${_target}.cpp ${libraw_LIB_SRCS})
    KDE4_ADD_EXECUTABLE(${_target} NOGUI ${${_target}_SRCS})
    TARGET_LINK_LIBRARIES(${_target} ${MATH_LIBRARY} ${LCMS_LIBRARIES} ${JPEG_LIBRARY} ${OPENMP_LDFLAGS})
    SET_TARGET_PROPERTIES(${_target} PROPERTIES COMPILE_FLAGS -DLIBRAW_BUILDLIB)
    IF(WIN32)
        TARGET_LINK_LIBRARIES(${_target} ws2_32)
    ENDIF(WIN32)
endmacro(libkdcraw_build_test)

SET(raw2png_SRCS raw2png.cpp)
KDE4_ADD_EXECUTABLE(raw2png NOGUI ${raw2png_SRCS})
TARGET_LINK_LIBRARIES(raw2png kdcraw)

libkdcraw_build_test( identify )
libkdcraw_build_test( simple_dcraw )
libkdcraw_build_test( mem_image )
libkdcraw_build_test( dcraw_emu )