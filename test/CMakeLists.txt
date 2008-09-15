
KDE4_NO_ENABLE_FINAL(kdcraw)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libkdcraw)

# Disable dcraw C Ansi code warnings from LibRaw. Just to be clear on the console.
FOREACH(_curentfile ${libraw_LIB_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w )
ENDFOREACH(_curentfile ${libraw_LIB_SRCS})

SET(raw2png_SRCS raw2png.cpp)
KDE4_ADD_EXECUTABLE(raw2png NOGUI ${raw2png_SRCS})
TARGET_LINK_LIBRARIES(raw2png kdcraw)

SET(identify_SRCS ../libraw/samples/identify.cpp ${libraw_LIB_SRCS})
KDE4_ADD_EXECUTABLE(identify NOGUI ${identify_SRCS})
TARGET_LINK_LIBRARIES(identify ${MATH_LIBRARY} ${LCMS_LIBRARIES} ${JPEG_LIBRARY})

SET(simple_dcraw_SRCS ../libraw/samples/simple_dcraw.cpp ${libraw_LIB_SRCS})
KDE4_ADD_EXECUTABLE(simple_dcraw NOGUI ${simple_dcraw_SRCS})
TARGET_LINK_LIBRARIES(simple_dcraw ${MATH_LIBRARY} ${LCMS_LIBRARIES} ${JPEG_LIBRARY})

SET(mem_image_SRCS ../libraw/samples/mem_image.cpp ${libraw_LIB_SRCS})
KDE4_ADD_EXECUTABLE(mem_image NOGUI ${simple_dcraw_SRCS})
TARGET_LINK_LIBRARIES(mem_image ${MATH_LIBRARY} ${LCMS_LIBRARIES} ${JPEG_LIBRARY})

SET(dcraw_emu_SRCS ../libraw/samples/dcraw_emu.cpp ${libraw_LIB_SRCS})
KDE4_ADD_EXECUTABLE(dcraw_emu NOGUI ${simple_dcraw_SRCS})
TARGET_LINK_LIBRARIES(dcraw_emu ${MATH_LIBRARY} ${LCMS_LIBRARIES} ${JPEG_LIBRARY})
