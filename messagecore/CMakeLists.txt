# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGECORE
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messagecore_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageCoreConfigVersion.cmake"
    SOVERSION 6
    )



# config-enterprise.h is needed for both ENTERPRISE_BUILD and BUILD_EVERYTHING
configure_file(src/config-enterprise.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config-enterprise.h )

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MessageCore")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMessageCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageCoreConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageCoreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageCoreConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6MessageCoreTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MessageCoreTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messagecore_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageCore COMPONENT Devel
    )

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

