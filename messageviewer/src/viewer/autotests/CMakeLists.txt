macro(add_messageviewer_viewer_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES Qt::Test KPim6::MessageViewer Qt::PrintSupport)
endmacro()

add_messageviewer_viewer_unittest(printmessagetest.cpp)
