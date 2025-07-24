# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_messageviewer_openurlwith_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KPim6::MessageViewer Qt::Test
)
endmacro()

add_messageviewer_openurlwith_unittest(openwithurlinfotest.cpp)
add_messageviewer_openurlwith_unittest(openurlwithmanagertest.cpp)
