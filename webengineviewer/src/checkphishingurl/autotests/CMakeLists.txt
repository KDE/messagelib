set(testwebengineviewer_webengine_common_SRCS)
ecm_qt_declare_logging_category(testwebengineviewer_webengine_common_SRCS HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer)

ecm_add_test(checkphishingurljobtest.cpp  ../checkphishingurljob.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurljobtest
    NAME_PREFIX "messageviewer-checkphishingurl-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )
