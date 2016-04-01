if (QTWEBENGINE_SUPPORT_OPTION)
    ecm_add_test(webengineembedparttest.cpp  ../webengineembedpart.cpp
        TEST_NAME webengineembedparttest
        NAME_PREFIX "messageviewer-webengine-"
        LINK_LIBRARIES Qt5::Test Qt5::Core
        )
endif()
