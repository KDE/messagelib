ecm_add_test(mailwebengineaccesskeyanchortest.cpp  ../mailwebengineaccesskeyanchor.cpp ../mailwebengineaccesskeyutils.cpp
    TEST_NAME mailwebengineaccesskeyanchortest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test KF5::MessageViewer
)

ecm_add_test(mailwebengineaccesskeyanchorfromhtmltest.cpp  ../mailwebengineaccesskeyanchor.cpp ../mailwebengineaccesskeyutils.cpp
    TEST_NAME mailwebengineaccesskeyanchorfromhtmltest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test KF5::MessageViewer
)
