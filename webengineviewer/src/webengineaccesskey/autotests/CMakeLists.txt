ecm_add_test(webengineaccesskeyanchortest.cpp  ../webengineaccesskeyanchor.cpp ../webengineaccesskeyutils.cpp
    TEST_NAME webengineaccesskeyanchortest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer
)

ecm_add_test(webengineaccesskeyanchorfromhtmltest.cpp  ../webengineaccesskeyanchor.cpp ../webengineaccesskeyutils.cpp
    TEST_NAME webengineaccesskeyanchorfromhtmltest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer Qt5::WebEngine Qt5::WebEngineWidgets
)
