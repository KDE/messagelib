# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_composer_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messagecomposer-composer-"
        LINK_LIBRARIES Qt::Test KPim6::MessageComposer
)
endmacro()

add_composer_unittest(composerattachmentinterfacetest.cpp)

