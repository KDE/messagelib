# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

include(CheckIncludeFile)
check_include_file(
    getopt.h
    HAVE_GETOPT_H
)

if(HAVE_GETOPT_H)
    add_executable(test_mdn test_mdn.cpp)
    add_test(NAME test_mdn COMMAND test_mdn)
    target_link_libraries(test_mdn KPim6::MessageCore)
endif()

add_executable(test_dates test_dates.cpp)
target_link_libraries(test_dates KPim6::MessageCore)
