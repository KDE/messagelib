# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

########### Targets ###########
add_subdirectory(src)
if(BUILD_TESTING)
    add_subdirectory(autotests)
    add_subdirectory(src/temporaryfile/autotests)
endif()

