set(testwebengineviewer_webengine_SRCS)
ecm_qt_declare_logging_category(testwebengineviewer_webengine_SRCS HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer)
include_directories(${Messagelib_BINARY_DIR})
ecm_add_test(webhittestresulttest.cpp
    TEST_NAME webhittestresulttest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer
)


ecm_add_test(zoomactionmenutest.cpp 
    TEST_NAME zoomactionmenutest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer KF5::XmlGui
)


ecm_add_test(webengineprintmessageboxtest.cpp ${testwebengineviewer_webengine_SRCS}
    TEST_NAME webengineprintmessageboxtest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer KF5::I18n
)

ecm_add_test(webengineexporthtmlpagejobtest.cpp ${testwebengineviewer_webengine_SRCS}
    TEST_NAME webengineexporthtmlpagejobtest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer
)
