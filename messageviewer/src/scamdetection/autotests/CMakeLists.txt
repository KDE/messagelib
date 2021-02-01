ecm_add_test(scamattributetest.cpp ../scamattribute.cpp
    TEST_NAME scamattributetest
    NAME_PREFIX "messageviewer-scamdetection-"
    LINK_LIBRARIES Qt5::Test KF5::AkonadiCore
    )

ecm_add_test(scamdetectionwebenginetest.cpp
    TEST_NAME scamdetectionwebenginetest
    NAME_PREFIX "messageviewer-scamwebenginedetection-"
    LINK_LIBRARIES KF5::MessageViewer Qt5::Test Qt5::WebEngine Qt5::WebEngineWidgets
    )
