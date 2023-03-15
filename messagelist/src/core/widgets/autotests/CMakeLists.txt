# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_messagelist_widget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messagelist-"
        LINK_LIBRARIES Qt::Test KPim${KF_MAJOR_VERSION}::MessageCore KPim${KF_MAJOR_VERSION}::MessageList KPim${KF_MAJOR_VERSION}::PimCommon KF${KF_MAJOR_VERSION}::IconThemes
    )
endmacro ()

add_messagelist_widget_unittest(quicksearchwarningtest.cpp)
add_messagelist_widget_unittest(searchlinestatustest.cpp)
add_messagelist_widget_unittest(searchcollectionindexingwarningtest.cpp)
add_messagelist_widget_unittest(tablockedwarningtest.cpp)
add_messagelist_widget_unittest(configurefilterswidgettest.cpp)
add_messagelist_widget_unittest(filternamewidgettest.cpp)

