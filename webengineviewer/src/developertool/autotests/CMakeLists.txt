# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

set(ECM_TEST_NAME_PREFIX "webengineviewer-")

macro(add_webengineviewer_widget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        LINK_LIBRARIES KPim6::WebEngineViewer Qt::Test
)
    ecm_test_get_name_prefix(test_name_prefix)
    # needed for the CI to run webengine-related tests
    # the CI runs as root, and webengine insists on disabling sandboxing in that case
    set_tests_properties(${test_name_prefix}${_name} PROPERTIES ENVIRONMENT "QTWEBENGINE_DISABLE_SANDBOX=1")

endmacro()

add_webengineviewer_widget_unittest(developertooldialogtest.cpp)
add_webengineviewer_widget_unittest(developertoolwidgettest.cpp)
