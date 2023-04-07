# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_setup_version(PROJECT VARIABLE_PREFIX WEBENGINEVIEWER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/webengineviewer_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6WebEngineViewerConfigVersion.cmake"
    SOVERSION 6
    )


########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6WebEngineViewer")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimWebEngineViewerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6WebEngineViewerConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6WebEngineViewerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6WebEngineViewerConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6WebEngineViewerTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6WebEngineViewerTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/src/webengineviewer_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/WebEngineViewer COMPONENT Devel
    )
include_directories(${Messagelib_BINARY_DIR})
add_subdirectory(src)

