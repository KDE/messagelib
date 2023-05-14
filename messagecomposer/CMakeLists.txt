# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGECOMPOSER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/messagecomposer_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageComposerConfigVersion.cmake"
    SOVERSION 6
    )

find_package(Inotify)
set_package_properties(Inotify PROPERTIES
                       PURPOSE "Filesystem alteration notifications using inotify")
if(Inotify_FOUND)
    set(HAVE_SYS_INOTIFY_H 1)
else()
    set(HAVE_SYS_INOTIFY_H 0)
endif()

########### CMake Config Files ###########
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MessageComposer")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMessageComposerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageComposerConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
    )

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageComposerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageComposerConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
    )

install(EXPORT KPim6MessageComposerTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MessageComposerTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/src/messagecomposer_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageComposer COMPONENT Devel
    )

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()
