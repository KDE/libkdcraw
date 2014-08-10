cmake_minimum_required(VERSION 2.8.12)

project(LibKDcraw)

find_package(ECM 1.1.0 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION 5.2.0)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Gui)

find_package(KF5 CONFIG REQUIRED
    KDELibs4Support
)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMMarkNonGuiExecutable)

set(KF5_VERSION "5.1.0") # handled by release scripts

ecm_setup_version(${KF5_VERSION} VARIABLE_PREFIX LIBKDCRAW
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/libkdcraw_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/LibKDcrawConfigVersion.cmake"
                        SOVERSION 5)

add_subdirectory(src)
# add_subdirectory(tests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/LibKDcraw")

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/LibKDcrawConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/LibKDcrawConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/LibKDcrawConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/LibKDcrawConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )


#TODO should I really be using Namespace KF5? Baloo does.
# install(EXPORT LibKDcrawTargets  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"  FILE LibKDcrawTargets.cmake  NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libkdcraw_version.h  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}  COMPONENT Devel )

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
