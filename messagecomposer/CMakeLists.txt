project(messagecomposer)

remove_definitions( -DQT_NO_CAST_FROM_BYTEARRAY )
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)

ecm_setup_version(${MESSAGELIB_LIB_VERSION} VARIABLE_PREFIX MESSAGECOMPOSER
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messagecomposer_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageComposerConfigVersion.cmake"
                        SOVERSION 5
)

configure_file(src/messagecomposer-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/messagecomposer-version.h @ONLY)
########### Targets ###########

add_definitions("-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII")

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5MessageComposer")

ecm_configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5MessageComposerConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageComposerConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageComposerConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5MessageComposerConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT KF5MessageComposerTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KF5MessageComposerTargets.cmake NAMESPACE KF5::)

install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer_version.h
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5} COMPONENT Devel
)

add_subdirectory(src)
if (BUILD_TESTING)
   add_subdirectory(autotests)
   add_subdirectory(tests)
endif()

