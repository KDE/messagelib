set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DHEADER_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

ecm_add_test(grantleeheaderstyletest.cpp
    TEST_NAME grantleeheaderstyletest
    NAME_PREFIX "messageviewer-header-"
    LINK_LIBRARIES
        KF5::GrantleeTheme
        KF5::IconThemes
        KF5::MessageViewer
        KF5::Mime
        Qt5::Test
)

ecm_add_test(grantleeheaderformattertest.cpp
    TEST_NAME grantleeheaderformattertest
    NAME_PREFIX "messageviewer-header-"
    LINK_LIBRARIES
        KF5::GrantleeTheme
        KF5::IconThemes
        KF5::MessageViewer
        KF5::Mime
        Qt5::Test
)
