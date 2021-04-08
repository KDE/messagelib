
macro(add_webengineviewer_widget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "webengineviewer-"
        LINK_LIBRARIES KF5::WebEngineViewer Qt::Test
    )
endmacro ()

add_webengineviewer_widget_unittest(developertooldialogtest.cpp)
add_webengineviewer_widget_unittest(developertoolwidgettest.cpp)
