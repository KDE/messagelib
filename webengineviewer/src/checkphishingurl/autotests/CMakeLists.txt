set(testwebengineviewer_webengine_common_SRCS)
ecm_qt_declare_logging_category(testwebengineviewer_webengine_common_SRCS HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer)
add_definitions( -DCHECKPHISHINGURL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

ecm_add_test(checkphishingurljobtest.cpp  ../checkphishingurljob.cpp ../updatedatabaseinfo.cpp ../checkphishingurlutil.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurljobtest
    NAME_PREFIX "webengineview-checkphishingurl-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )

ecm_add_test(checkphishingurlfromlocaldatabasejobtest.cpp  ../checkphishingurlutil.cpp ../updatedatabaseinfo.cpp ../checkphishingurlfromlocaldatabasejob.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurlfromlocaldatabasetest
    NAME_PREFIX "webengineview-checkphishingurlfromlocaldatabasejob-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )


ecm_add_test(createphishingurldatabasejobtest.cpp ../checkphishingurlutil.cpp ../updatedatabaseinfo.cpp ../createphishingurldatabasejob.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME createphishingurldatabasejobtest
    NAME_PREFIX "webengineview-createphishingurldatabasejob-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )



ecm_add_test(localdatabasemanagertest.cpp ../checkphishingurlutil.cpp ../updatedatabaseinfo.cpp ../localdatabasemanager.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME localdatabasemanagertest
    NAME_PREFIX "webengineview-localdatabasemanager-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )

ecm_add_test(searchfullhashjobtest.cpp ../checkphishingurlutil.cpp ../updatedatabaseinfo.cpp ../searchfullhashjob.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME searchfullhashjobtest
    NAME_PREFIX "webengineview-searchfullhashjob-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )

ecm_add_test(localdatabasefiletest.cpp ../localdatabasefile.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME localdatabasefiletest
    NAME_PREFIX "webengineview-localdatabasefile-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )

ecm_add_test(createdatabasefilejobtest.cpp ../createdatabasefilejob.cpp ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME createdatabasefilejobtest
    NAME_PREFIX "webengineview-createdatabasefilejob-"
    LINK_LIBRARIES Qt5::Test Qt5::WebKitWidgets KF5::WebEngineViewer
    )
