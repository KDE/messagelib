project(MimeTreeParser)

ecm_setup_version(${MESSAGELIB_LIB_VERSION} VARIABLE_PREFIX MIMETREEPARSER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5MimeTreeParserConfigVersion.cmake"
    SOVERSION 5
    )


########### Targets ###########

add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MimeTreeParser")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5MimeTreeParserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MimeTreeParserConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MimeTreeParserConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MimeTreeParserConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF5MimeTreeParserTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5MimeTreeParserTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/mimetreeparser_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(src/temporaryfile/autotests)
endif()

