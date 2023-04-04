# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_setup_version(PROJECT VARIABLE_PREFIX MIMETREEPARSER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MimeTreeParserConfigVersion.cmake"
    SOVERSION 6
    )


########### Targets ###########

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MimeTreeParser")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMimeTreeParserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MimeTreeParserConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MimeTreeParserConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MimeTreeParserConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6MimeTreeParserTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MimeTreeParserTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MimeTreeParser COMPONENT Devel
    )

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(src/temporaryfile/autotests)
endif()

