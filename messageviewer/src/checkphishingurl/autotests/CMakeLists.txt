ecm_add_test(checkphishingurljobtest.cpp  ../checkphishingurljob.cpp
    TEST_NAME checkphishingurljobtest
    NAME_PREFIX "messageviewer-checkphishingurl-"
    LINK_LIBRARIES Qt5::Test KF5::MimeTreeParser KF5::MessageViewer Qt5::WebKitWidgets KF5::WebEngineViewer
    )
