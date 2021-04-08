macro(add_messageviewer_remote_contents_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::WebEngineViewer KF5::Libkleo QGpgme Qt::Test qca-qt5 KF5::ItemViews
    )
endmacro ()

add_messageviewer_remote_contents_unittest(remotecontentinfotest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentconfiguredialogtest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentconfigurewidgettest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentmanagertest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentwidgettest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentdialogtest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentmenutest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentstatustypecomboboxtest.cpp)
