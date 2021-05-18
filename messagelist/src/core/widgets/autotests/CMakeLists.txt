macro(add_messagelist_widget_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source}
        TEST_NAME ${_name}
        NAME_PREFIX "messagelist-"
	LINK_LIBRARIES KF5::MessageList Qt::Test
    )
endmacro ()

#add_messagelist_widget_unittest(shownextmessagewidgettest.cpp)

