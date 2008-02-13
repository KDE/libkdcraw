INCLUDE_DIRECTORIES(${KDE4_INCLUDES} ${QT4_INCLUDES})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/version.h)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/dcrawinfo.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/dcrawinfo.h)

SET(kdcraw_LIB_SRCS dcrawbinary.cpp 
                    kdcraw.cpp 
                    dcrawsettingswidget.cpp)

KDE4_ADD_LIBRARY(kdcraw SHARED ${kdcraw_LIB_SRCS})

TARGET_LINK_LIBRARIES(kdcraw ${KDE4_KDEUI_LIBS})

SET_TARGET_PROPERTIES(kdcraw PROPERTIES VERSION ${DCRAW_LIB_SO_VERSION_STRING} SOVERSION ${DCRAW_LIB_SO_CUR_VERSION})

INSTALL(TARGETS kdcraw DESTINATION ${LIB_INSTALL_DIR})

INSTALL(FILES rawdecodingsettings.h 
              kdcraw.h 
              dcrawbinary.h 
              dcrawsettingswidget.h 
              dcrawinfocontainer.h 
              rawfiles.h 
              libkdcraw_export.h 
              ${CMAKE_CURRENT_BINARY_DIR}/version.h 
        DESTINATION ${INCLUDE_INSTALL_DIR}/libkdcraw)
