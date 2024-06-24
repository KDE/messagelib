# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

add_executable(test_dates test_dates.cpp)
add_test(NAME test_dates COMMAND test_dates)
target_link_libraries(test_dates KPim6::MessageCore)
