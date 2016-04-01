ecm_add_test(webengineaccesskeyanchortest.cpp  ../webengineaccesskeyanchor.cpp ../webengineaccesskeyutils.cpp
    TEST_NAME webengineaccesskeyanchortest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test KF5::MessageViewer
)

ecm_add_test(webengineaccesskeyanchorfromhtmltest.cpp  ../webengineaccesskeyanchor.cpp ../webengineaccesskeyutils.cpp
    TEST_NAME webengineaccesskeyanchorfromhtmltest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test KF5::MessageViewer Qt5::WebEngine Qt5::WebEngineWidgets
)
