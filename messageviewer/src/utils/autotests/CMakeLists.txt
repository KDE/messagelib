add_definitions( -DMESSAGEVIEWER_UTIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )
macro(add_messageviewer_utils_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KF5::MessageViewer KF5::WebEngineViewer KF5::Libkleo QGpgme Qt::Test
    )
endmacro ()

add_messageviewer_utils_unittest(messageviewerutilstest.cpp)
