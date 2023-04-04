# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGEVIEWER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messageviewer_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageViewerConfigVersion.cmake"
    SOVERSION 6
    )


########### Targets ###########

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MessageViewer")


configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMessageViewerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageViewerConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageViewerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageViewerConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6MessageViewerTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MessageViewerTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messageviewer_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageViewer COMPONENT Devel
    )

include_directories(${Messagelib_BINARY_DIR})

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()

