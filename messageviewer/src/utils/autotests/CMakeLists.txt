# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_definitions(-DMESSAGEVIEWER_UTIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
macro(add_messageviewer_utils_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KPim6::MessageViewer KPim6::WebEngineViewer KPim6::Libkleo QGpgmeQt6 Qt::Test
)
endmacro()


add_messageviewer_utils_unittest(messageviewerutilstest.cpp)
