ecm_add_test(mailwebengineaccesskeyanchortest.cpp  ../mailwebengineaccesskeyanchor.cpp
    TEST_NAME mailwebengineaccesskeyanchortest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test KF5::MessageViewer
)
