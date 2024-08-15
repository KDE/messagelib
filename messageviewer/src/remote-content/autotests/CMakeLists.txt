# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_messageviewer_remote_contents_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KPim6::MessageViewer KPim6::WebEngineViewer KPim6::Libkleo QGpgmeQt6 Qt::Test qca-qt6 KF6::ItemViews
)
endmacro()

add_messageviewer_remote_contents_unittest(remotecontentinfotest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentconfiguredialogtest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentconfigurewidgettest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentmanagertest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentwidgettest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentdialogtest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentmenutest.cpp)
add_messageviewer_remote_contents_unittest(remotecontentstatustypecomboboxtest.cpp)
