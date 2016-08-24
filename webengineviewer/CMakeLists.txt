project(WebEngineViewer)

ecm_setup_version(${MESSAGELIB_LIB_VERSION} VARIABLE_PREFIX WEBENGINEVIEWER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/webengineviewer_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5WebEngineViewerConfigVersion.cmake"
    SOVERSION 5
    )


########### Targets ###########
add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5WebEngineViewer")

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5WebEngineViewerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5WebEngineViewerConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5WebEngineViewerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5WebEngineViewerConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF5WebEngineViewerTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5WebEngineViewerTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/webengineviewer_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )
include_directories(${Messagelib_BINARY_DIR})
add_subdirectory(src)

