# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_webengineviewer_widget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "webengineviewer-"
        LINK_LIBRARIES KPim6::WebEngineViewer Qt::Test
)
    # needed for the CI to run webengine-related tests
    # the CI runs as root, and webengine insists on disabling sandboxing in that case
    set_tests_properties(webengineviewer-${_name} PROPERTIES ENVIRONMENT "QTWEBENGINE_DISABLE_SANDBOX=1")

endmacro()

add_webengineviewer_widget_unittest(developertooldialogtest.cpp)
add_webengineviewer_widget_unittest(developertoolwidgettest.cpp)
