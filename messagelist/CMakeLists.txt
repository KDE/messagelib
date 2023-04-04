# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGELIST
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messagelist_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageListConfigVersion.cmake"
    SOVERSION 6
    )

########### Targets ###########

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MessageList")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMessageListConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageListConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageListConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageListConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6MessageListTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MessageListTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messagelist_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageList COMPONENT Devel
    )

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

