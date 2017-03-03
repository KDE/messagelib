
macro(add_messageviewer_webengine_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::Libkleo QGpgme Qt5::Test KF5::KIOCore KF5::Mime KF5::AkonadiCore KF5::CalendarCore
    )
endmacro ()

add_messageviewer_webengine_unittest(mailwebenginescripttest.cpp)

