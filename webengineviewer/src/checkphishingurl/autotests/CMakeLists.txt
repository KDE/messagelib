set(testwebengineviewer_webengine_common_SRCS)
ecm_qt_declare_logging_category(testwebengineviewer_webengine_common_SRCS HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer)
add_definitions( -DCHECKPHISHINGURL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

ecm_add_test(checkphishingurljobtest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurljobtest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(createphishingurldatabasejobtest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME createphishingurldatabasejobtest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(localdatabasemanagertest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME localdatabasemanagertest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(searchfullhashjobtest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME searchfullhashjobtest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(localdatabasefiletest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME localdatabasefiletest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(createdatabasefilejobtest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME createdatabasefilejobtest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(verifydatabaseupdatetest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME verifydatabaseupdatetest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )


ecm_add_test(urlhashingtest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME urlhashingtest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(riceencodingdecodertest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME riceencodingdecodertest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(hashcachemanagertest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME hashcachemanagertest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::ConfigCore KF5::WebEngineViewer
    )

ecm_add_test(checkphishingurlutiltest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurlutiltest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::WebEngineViewer
    )

ecm_add_test(checkphishingurlcachetest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurlcachetest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::ConfigCore KF5::WebEngineViewer
    )

ecm_add_test(backoffmodemanagertest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME backoffmodemanagertest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test KF5::ConfigCore KF5::WebEngineViewer
    )

ecm_add_test(downloadlocaldatabasethreadtest.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME downloadlocaldatabasethreadtest
    NAME_PREFIX "webengineview-"
    LINK_LIBRARIES Qt::Test Qt::Network KF5::WebEngineViewer
    )
