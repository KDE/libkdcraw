include_directories(${KDE4_INCLUDES} ${QT4_INCLUDES})

set(kdcraw_LIB_SRCS dcrawbinary.cpp kdcraw.cpp dcrawsettingswidget.cpp)

kde4_automoc(${kdcraw_LIB_SRCS})

kde4_add_library(kdcraw SHARED ${kdcraw_LIB_SRCS})

target_link_libraries(kdcraw ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} lcms)

set_target_properties(kdcraw PROPERTIES VERSION 2.0.0 SOVERSION 2)

install(TARGETS kdcraw DESTINATION ${LIB_INSTALL_DIR})

install(FILES rawdecodingsettings.h kdcraw.h dcrawbinary.h dcrawsettingswidget.h dcrawinfocontainer.h rawfiles.h libkdcraw_export.h version.h DESTINATION ${INCLUDE_INSTALL_DIR}/libkdcraw)

