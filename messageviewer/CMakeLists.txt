ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGEVIEWER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messageviewer_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageViewerConfigVersion.cmake"
    SOVERSION 5
    )


########### Targets ###########

add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MessageViewer")


configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5MessageViewerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageViewerConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageViewerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageViewerConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF5MessageViewerTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5MessageViewerTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messageviewer_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )

include_directories(${Messagelib_BINARY_DIR})

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()

