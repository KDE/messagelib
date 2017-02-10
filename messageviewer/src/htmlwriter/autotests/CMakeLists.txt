ecm_add_test(webengineembedparttest.cpp  ../webengineembedpart.cpp
    TEST_NAME webengineembedparttest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test Qt5::Core
    )

set(webengineparthtmlwritertestdebug_SRCS)
ecm_qt_declare_logging_category(webengineparthtmlwritertestdebug_SRCS HEADER messageviewer_debug.h IDENTIFIER MESSAGEVIEWER_LOG CATEGORY_NAME org.kde.pim.messageviewer)


ecm_add_test(webengineparthtmlwritertest.cpp  ../webengineembedpart.cpp ../webengineparthtmlwriter.cpp ${webengineparthtmlwritertestdebug_SRCS}
    TEST_NAME webengineparthtmlwritertest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt5::Test Qt5::Core KF5::MimeTreeParser KF5::MessageViewer KF5::WebEngineViewer
    )
