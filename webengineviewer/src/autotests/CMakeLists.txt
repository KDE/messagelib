ecm_add_test(webhittestresulttest.cpp  ../webhittestresult.cpp 
    TEST_NAME webhittestresulttest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer
)


ecm_add_test(zoomactionmenutest.cpp 
    TEST_NAME zoomactionmenutest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer KF5::XmlGui
)


ecm_add_test(webengineprintmessageboxtest.cpp
    TEST_NAME webengineprintmessageboxtest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer
)

ecm_add_test(webengineexporthtmlpagejobtest.cpp
    TEST_NAME webengineexporthtmlpagejobtest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer
)
