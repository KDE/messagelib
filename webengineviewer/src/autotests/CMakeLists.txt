# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(testwebengineviewer_webengine_SRCS)
ecm_qt_declare_logging_category(testwebengineviewer_webengine_SRCS HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer)

include_directories(${Messagelib_BINARY_DIR})
ecm_add_test(webhittestresulttest.cpp webhittestresulttest.h
    TEST_NAME webhittestresulttest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)


ecm_add_test(zoomactionmenutest.cpp zoomactionmenutest.h
    TEST_NAME zoomactionmenutest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer KF6::XmlGui
)

ecm_add_test(webengineexportpdfpagejobtest.cpp webengineexportpdfpagejobtest.h ${testwebengineviewer_webengine_SRCS}
    TEST_NAME webengineexportpdfpagejobtest
    NAME_PREFIX "webengineviewer-"
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)
