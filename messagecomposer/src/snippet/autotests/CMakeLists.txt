# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_composer_snippet_unittest _source)
    ecm_add_test(${_source}
        NAME_PREFIX "messagecomposer-snippet-"
        LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::MessageComposer KF5::Completion
    )
endmacro()

add_composer_snippet_unittest(convertvariablesjobtest.cpp)
add_composer_snippet_unittest(convertsnippetvariablesutiltest.cpp)
add_composer_snippet_unittest(convertsnippetvariablemenutest.cpp)
