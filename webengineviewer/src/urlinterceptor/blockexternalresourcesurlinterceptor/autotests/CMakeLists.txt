# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_messageviewer_blockexternalresource_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::WebEngineViewer Qt${QT_MAJOR_VERSION}::Test
    )
endmacro ()

add_messageviewer_blockexternalresource_unittest(blockexternalresourcesurlinterceptortest.cpp)
