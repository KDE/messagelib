set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DHEADER_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )
add_definitions( -DGRANTLEETHEME_DATA_DIR="${CMAKE_SOURCE_DIR}/messageviewer/src/messageviewerheaderplugins/defaultgrantleeheaderstyleplugin/theme/" )

ecm_add_test(grantleeheaderstyletest.cpp
    TEST_NAME grantleeheaderstyletest
    NAME_PREFIX "messageviewer-header-"
    LINK_LIBRARIES
        KF5::GrantleeTheme
        KF5::IconThemes
        KF5::MessageViewer
        KF5::Mime
        Qt::Test
)

ecm_add_test(grantleeheaderformattertest.cpp
    TEST_NAME grantleeheaderformattertest
    NAME_PREFIX "messageviewer-header-"
    LINK_LIBRARIES
        Grantlee5::Templates
        KF5::GrantleeTheme
        KF5::IconThemes
        KF5::MessageViewer
        KF5::Mime
        Qt::Test
)

set_tests_properties(
    messageviewer-header-grantleeheaderstyletest
    messageviewer-header-grantleeheaderformattertest
    PROPERTIES ENVIRONMENT "LC_ALL=C;TZ=UTC"
)
