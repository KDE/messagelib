macro(add_messageviewer_blockexternalresource_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::WebEngineViewer Qt::Test
    )
endmacro ()

add_messageviewer_blockexternalresource_unittest(blockexternalresourcesurlinterceptortest.cpp)
