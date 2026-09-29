# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_messageviewer_dkim_verify_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        LINK_LIBRARIES KPim6::MessageViewer KPim6::WebEngineViewer KPim6::Libkleo QGpgmeQt6 Qt::Test KF6::ItemViews
    )
endmacro()

add_messageviewer_dkim_verify_unittest(dkimmanagerkeywidgettest.cpp)
add_messageviewer_dkim_verify_unittest(dkimmanagerkeydialogtest.cpp)
add_messageviewer_dkim_verify_unittest(dkimwidgetinfotest.cpp)
add_messageviewer_dkim_verify_unittest(dkimviewermenutest.cpp)
add_messageviewer_dkim_verify_unittest(dkimmanagerulescomboboxtest.cpp)
add_messageviewer_dkim_verify_unittest(dkimmanagerulesdialogtest.cpp)
add_messageviewer_dkim_verify_unittest(dkimmanageruleswidgettest.cpp)
add_messageviewer_dkim_verify_unittest(dkimruledialogtest.cpp)
add_messageviewer_dkim_verify_unittest(dkimrulewidgettest.cpp)
add_messageviewer_dkim_verify_unittest(dkimmanagerkeytreeviewtest.cpp)
