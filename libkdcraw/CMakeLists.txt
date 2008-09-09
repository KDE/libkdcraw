
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

SET(kdcraw_LIB_SRCS kdcraw.cpp 
                    dcrawsettingswidget.cpp
                    rcombobox.cpp
                    rnuminput.cpp
                    ${libraw_LIB_SRCS}
   )

KDE4_ADD_LIBRARY(kdcraw SHARED ${kdcraw_LIB_SRCS})

TARGET_LINK_LIBRARIES(kdcraw ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${MATH_LIBRARY} ${LCMS_LIBRARIES} ${JPEG_LIBRARY})

SET_TARGET_PROPERTIES(kdcraw PROPERTIES VERSION ${DCRAW_LIB_SO_VERSION_STRING} SOVERSION ${DCRAW_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kdcraw ${INSTALL_TARGETS_DEFAULT_ARGS})

INSTALL(FILES rawdecodingsettings.h 
              kdcraw.h 
              dcrawsettingswidget.h
              rnuminput.h
              rcombobox.h 
              dcrawinfocontainer.h 
              rawfiles.h 
              libkdcraw_export.h 
              ${CMAKE_CURRENT_BINARY_DIR}/version.h 
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkdcraw COMPONENT Devel)
