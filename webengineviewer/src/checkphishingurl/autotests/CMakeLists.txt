# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(testwebengineviewer_webengine_common_SRCS)
ecm_qt_declare_logging_category(testwebengineviewer_webengine_common_SRCS HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer)
add_definitions(-DCHECKPHISHINGURL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

ecm_add_test(checkphishingurljobtest.cpp checkphishingurljobtest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurljobtest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(createphishingurldatabasejobtest.cpp createphishingurldatabasejobtest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME createphishingurldatabasejobtest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(localdatabasemanagertest.cpp localdatabasemanagertest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME localdatabasemanagertest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(searchfullhashjobtest.cpp searchfullhashjobtest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME searchfullhashjobtest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(localdatabasefiletest.cpp localdatabasefiletest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME localdatabasefiletest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(createdatabasefilejobtest.cpp createdatabasefilejobtest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME createdatabasefilejobtest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(verifydatabaseupdatetest.cpp verifydatabaseupdatetest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME verifydatabaseupdatetest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)


ecm_add_test(urlhashingtest.cpp urlhashingtest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME urlhashingtest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(riceencodingdecodertest.cpp riceencodingdecodertest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME riceencodingdecodertest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(hashcachemanagertest.cpp hashcachemanagertest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME hashcachemanagertest
    LINK_LIBRARIES Qt::Test KF6::ConfigCore KPim6::WebEngineViewer
)

ecm_add_test(checkphishingurlutiltest.cpp checkphishingurlutiltest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurlutiltest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)

ecm_add_test(checkphishingurlcachetest.cpp checkphishingurlcachetest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME checkphishingurlcachetest
    LINK_LIBRARIES Qt::Test KF6::ConfigCore KPim6::WebEngineViewer
)

ecm_add_test(backoffmodemanagertest.cpp backoffmodemanagertest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME backoffmodemanagertest
    LINK_LIBRARIES Qt::Test KF6::ConfigCore KPim6::WebEngineViewer
)

ecm_add_test(downloadlocaldatabasethreadtest.cpp downloadlocaldatabasethreadtest.h ${testwebengineviewer_webengine_common_SRCS}
    TEST_NAME downloadlocaldatabasethreadtest
    LINK_LIBRARIES Qt::Test Qt::Network KPim6::WebEngineViewer
)
