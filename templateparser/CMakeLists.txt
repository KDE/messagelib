# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

ecm_setup_version(PROJECT VARIABLE_PREFIX TEMPLATEPARSER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/templateparser_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6TemplateParserConfigVersion.cmake"
    SOVERSION 6
    )

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6TemplateParser")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimTemplateParserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6TemplateParserConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6TemplateParserConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6TemplateParserConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6TemplateParserTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6TemplateParserTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/templateparser_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/TemplateParser COMPONENT Devel
    )

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()

