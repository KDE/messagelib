# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
if (QT_MAJOR_VERSION STREQUAL "6")
    set(qgpgme_lib QGpgmeQt6)
else()
    set(qgpgme_lib QGpgme)
endif()
macro(add_messageviewer_remote_contents_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KPim${KF_MAJOR_VERSION}::MessageViewer KPim${KF_MAJOR_VERSION}::WebEngineViewer KPim${KF_MAJOR_VERSION}::Libkleo ${qgpgme_lib} Qt::Test qca-qt${QT_MAJOR_VERSION} KF${KF_MAJOR_VERSION}::ItemViews
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
