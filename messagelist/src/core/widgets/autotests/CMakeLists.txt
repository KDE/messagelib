macro(add_messagelist_widget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "messagelist-"
        LINK_LIBRARIES Qt::Test KF5::MessageCore KF5::MessageList KF5::PimCommon KF5::IconThemes
    )
endmacro ()

add_messagelist_widget_unittest(quicksearchwarningtest.cpp)
add_messagelist_widget_unittest(searchlinestatustest.cpp)
add_messagelist_widget_unittest(searchcollectionindexingwarningtest.cpp)
add_messagelist_widget_unittest(tablockedwarningtest.cpp)
add_messagelist_widget_unittest(configurefilterswidgettest.cpp)
add_messagelist_widget_unittest(filternamewidgettest.cpp)

