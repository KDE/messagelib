# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
macro(add_messagelist_widget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messagelist-"
        LINK_LIBRARIES Qt::Test KPim6::MessageCore KPim6::MessageList KPim6::PimCommon KF6::IconThemes KF6::IconWidgets
)
endmacro()

add_messagelist_widget_unittest(quicksearchwarningtest.cpp)
add_messagelist_widget_unittest(searchlinestatustest.cpp)
add_messagelist_widget_unittest(searchcollectionindexingwarningtest.cpp)
add_messagelist_widget_unittest(tablockedwarningtest.cpp)
add_messagelist_widget_unittest(configurefilterswidgettest.cpp)
add_messagelist_widget_unittest(filternamewidgettest.cpp)

add_messagelist_widget_unittest(searchstatusbuttonstest.cpp)
add_messagelist_widget_unittest(searchmessagebybuttonstest.cpp)
add_messagelist_widget_unittest(searchlinecommandtest.cpp)
add_messagelist_widget_unittest(searchlinecommandbuttonswidgettest.cpp)
