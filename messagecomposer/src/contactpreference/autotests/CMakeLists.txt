# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

macro(add_composerng_autocorrect_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messagecomposer-contactpreference-"
        LINK_LIBRARIES Qt::Test KPim6::MessageComposer
)
endmacro()

add_composerng_autocorrect_unittest(contactpreferencetest.cpp)
