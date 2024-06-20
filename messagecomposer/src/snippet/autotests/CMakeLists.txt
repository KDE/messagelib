# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_composer_snippet_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messagecomposer-snippet-"
        LINK_LIBRARIES Qt::Test KPim6::MessageComposer KF6::Completion
)
endmacro()

add_composer_snippet_unittest(convertvariablesjobtest.cpp)
add_composer_snippet_unittest(convertsnippetvariablesutiltest.cpp)
add_composer_snippet_unittest(convertsnippetvariablemenutest.cpp)
