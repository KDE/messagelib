ecm_add_test(checkphishingurljobtest.cpp  ../checkphishingurljob.cpp
    TEST_NAME checkphishingurljobtest
    NAME_PREFIX "messageviewer-checkphishingurl-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )
