# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DHEADER_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions(-DGRANTLEETHEME_DATA_DIR="${CMAKE_SOURCE_DIR}/messageviewer/src/messageviewerheaderplugins/defaultgrantleeheaderstyleplugin/theme/")

ecm_add_test(grantleeheaderstyletest.cpp grantleeheaderstyletest.h
    TEST_NAME grantleeheaderstyletest
    NAME_PREFIX "messageviewer-header-"
    LINK_LIBRARIES
        KPim6::GrantleeTheme
        KF6::IconThemes
        KPim6::MessageViewer
        KPim6::Mime
        Qt::Test
)

ecm_add_test(grantleeheaderformattertest.cpp grantleeheaderformattertest.h
    TEST_NAME grantleeheaderformattertest
    NAME_PREFIX "messageviewer-header-"
    LINK_LIBRARIES
        KF6::TextTemplate
        KPim6::GrantleeTheme
        KF6::IconThemes
        KPim6::MessageViewer
        KPim6::Mime
        Qt::Test
)

set_tests_properties(
    messageviewer-header-grantleeheaderstyletest
    messageviewer-header-grantleeheaderformattertest
    PROPERTIES ENVIRONMENT "LC_ALL=en_US.UTF-8;TZ=UTC"
)
