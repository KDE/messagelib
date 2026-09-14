# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_webengineviewer_blocktracking_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        LINK_LIBRARIES KPim6::WebEngineViewer Qt::Test
    )
endmacro()

add_webengineviewer_blocktracking_unittest(blocktrackingurlinterceptortest.cpp)
