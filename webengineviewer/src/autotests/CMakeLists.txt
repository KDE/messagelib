ecm_add_test(webhittestresulttest.cpp  ../webhittestresult.cpp 
    TEST_NAME webhittestresulttest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt5::Test KF5::WebEngineViewer
)


