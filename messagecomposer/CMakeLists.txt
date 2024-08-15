# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none


find_package(Inotify)
set_package_properties(Inotify PROPERTIES
    PURPOSE "Filesystem alteration notifications using inotify")
if(Inotify_FOUND)
    set(HAVE_SYS_INOTIFY_H 1)
else()
    set(HAVE_SYS_INOTIFY_H 0)
endif()

configure_file(config-messagecomposer.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/config-messagecomposer.h)
add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()
