# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
if (QT_MAJOR_VERSION STREQUAL "6")
    set(qgpgme_lib QGpgmeQt6)
else()
    set(qgpgme_lib QGpgme)
endif()
add_definitions( -DMESSAGEVIEWER_UTIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )
macro(add_messageviewer_utils_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messageviewer-"
        LINK_LIBRARIES KPim${KF_MAJOR_VERSION}::MessageViewer KPim${KF_MAJOR_VERSION}::WebEngineViewer KPim${KF_MAJOR_VERSION}::Libkleo ${qgpgme_lib} Qt::Test
    )
    if (TARGET Qt::Core5Compat)
        target_link_libraries(${_name} Qt::Core5Compat)
    endif()
endmacro ()


add_messageviewer_utils_unittest(messageviewerutilstest.cpp)
