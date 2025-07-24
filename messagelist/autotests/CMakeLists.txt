# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
# convenience macro to add qtest unit tests
macro(messagelist_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} 
        TEST_NAME ${_name}
        NAME_PREFIX "messagelist-"
        LINK_LIBRARIES Qt::Test KPim6::MessageCore KPim6::MessageList KPim6::PimCommonAkonadi
)
endmacro()

messagelist_unittest(itemsizetest.cpp)
