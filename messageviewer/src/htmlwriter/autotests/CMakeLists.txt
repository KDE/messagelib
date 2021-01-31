ecm_add_test(webengineembedparttest.cpp  ../webengineembedpart.cpp
    TEST_NAME webengineembedparttest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt::Test Qt::Core
    )

