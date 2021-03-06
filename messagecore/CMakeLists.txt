ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGECORE
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messagecore_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageCoreConfigVersion.cmake"
    SOVERSION 5
    )



# config-enterprise.h is needed for both ENTERPRISE_BUILD and BUILD_EVERYTHING
configure_file(src/config-enterprise.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config-enterprise.h )

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MessageCore")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5MessageCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageCoreConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageCoreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageCoreConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KF5MessageCoreTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5MessageCoreTargets.cmake NAMESPACE KF5::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messagecore_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
    )

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

