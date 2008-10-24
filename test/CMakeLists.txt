KDE4_NO_ENABLE_FINAL(kdcraw)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkdcraw)

# add a small macro so that this is a bit cleaner
MACRO(libkdcraw_build_test)
    SET(_target ${ARGV0})
    SET(${_target}_SRCS 
        ../libraw/samples/${_target}
        ${libraw_LIB_SRCS}
       )
    
    # Disable dcraw C Ansi code warnings from LibRaw. Just to be clear on the console.
    FOREACH(_curentfile ${${_target}_SRCS})
	SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w)
    ENDFOREACH(_curentfile ${${_target}_SRCS})

    KDE4_ADD_EXECUTABLE(${_target} NOGUI ${${_target}_SRCS})
    TARGET_LINK_LIBRARIES(${_target} ${MATH_LIBRARY} ${LCMS_LIBRARIES} ${JPEG_LIBRARY} ${OPENMP_LDFLAGS})
    SET_TARGET_PROPERTIES(${_target} PROPERTIES COMPILE_FLAGS -DLIBRAW_BUILDLIB)
    IF(WIN32)
        TARGET_LINK_LIBRARIES(${_target} ws2_32)
    ENDIF(WIN32)
ENDMACRO(libkdcraw_build_test)

SET(raw2png_SRCS raw2png.cpp)
KDE4_ADD_EXECUTABLE(raw2png NOGUI ${raw2png_SRCS})
TARGET_LINK_LIBRARIES(raw2png kdcraw)

LIBKDCRAW_BUILD_TEST(identify.cpp)
LIBKDCRAW_BUILD_TEST(simple_dcraw.cpp)
LIBKDCRAW_BUILD_TEST(mem_image.cpp)
LIBKDCRAW_BUILD_TEST(dcraw_emu.cpp)
LIBKDCRAW_BUILD_TEST(dcraw_half.cpp)
