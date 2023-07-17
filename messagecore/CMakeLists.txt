# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none




# config-enterprise.h is needed for both ENTERPRISE_BUILD and BUILD_EVERYTHING
configure_file(src/config-enterprise.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config-enterprise.h )

########### CMake Config Files ###########

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(autotests)
endif()

