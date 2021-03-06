ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGELIST
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messagelist_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageListConfigVersion.cmake"
    SOVERSION 5
    )

########### Targets ###########

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MessageList")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5MessageListConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageListConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageListConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageListConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF5MessageListTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5MessageListTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messagelist_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

