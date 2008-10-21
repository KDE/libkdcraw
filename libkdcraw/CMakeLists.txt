
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

# Disable dcraw C Ansi code warnings from LibRaw. Just to be clear on the console.
FOREACH(_curentfile ${libraw_LIB_SRCS})
    SET_SOURCE_FILES_PROPERTIES(${_curentfile} PROPERTIES COMPILE_FLAGS -w )
ENDFOREACH(_curentfile ${libraw_LIB_SRCS})

SET(kdcraw_LIB_SRCS kdcraw.cpp
                    kdcrawprivate.cpp
                    dcrawsettingswidget.cpp
                    rcombobox.cpp
                    rnuminput.cpp
                    squeezedcombobox.cpp
                    ${libraw_LIB_SRCS}
   )
   
KDE4_ADD_LIBRARY(kdcraw SHARED ${kdcraw_LIB_SRCS})

TARGET_LINK_LIBRARIES(kdcraw ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${MATH_LIBRARY} ${LCMS_LIBRARIES} ${JPEG_LIBRARY} ${OPENMP_LDFLAGS})

SET_TARGET_PROPERTIES(kdcraw PROPERTIES VERSION ${DCRAW_LIB_SO_VERSION_STRING} SOVERSION ${DCRAW_LIB_SO_CUR_VERSION}
                                        DEFINE_SYMBOL LIBRAW_BUILDLIB)

INSTALL(TARGETS kdcraw ${INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(FILES rawdecodingsettings.h 
              kdcraw.h 
              dcrawsettingswidget.h
              rnuminput.h
              rcombobox.h 
              squeezedcombobox.h
              dcrawinfocontainer.h 
              rawfiles.h 
              libkdcraw_export.h 
              ${CMAKE_CURRENT_BINARY_DIR}/version.h 
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkdcraw COMPONENT Devel)
