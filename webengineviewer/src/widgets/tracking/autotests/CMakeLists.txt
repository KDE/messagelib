# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_webengine_widget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "webengineviewer-"
        LINK_LIBRARIES KPim6::WebEngineViewer Qt::Test
)
endmacro()

add_webengine_widget_unittest(trackingwarningwidgettest.cpp)
add_webengine_widget_unittest(trackingdetailsdialogtest.cpp)
