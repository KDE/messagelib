# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none


########### CMake Config Files ###########

include_directories(${Messagelib_BINARY_DIR})

add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(tests)
endif()
