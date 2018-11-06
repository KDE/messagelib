macro(add_messageviewer_dkim_verify_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::WebEngineViewer KF5::Libkleo QGpgme Qt5::Test
    )
endmacro ()

add_messageviewer_dkim_verify_unittest(dkiminfotest.cpp)
add_messageviewer_dkim_verify_unittest(dkimmanagerkeytest.cpp)
add_messageviewer_dkim_verify_unittest(dkimmanagerkeywidgettest.cpp)
add_messageviewer_dkim_verify_unittest(dkimmanagerkeydialogtest.cpp)
add_messageviewer_dkim_verify_unittest(dkimdownloadkeyjobtest.cpp)
add_messageviewer_dkim_verify_unittest(dkimconfiguredialogtest.cpp)
add_messageviewer_dkim_verify_unittest(dkimconfigurewidgettest.cpp)
add_messageviewer_dkim_verify_unittest(dkimchecksignaturejobtest.cpp)
