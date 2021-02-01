macro(add_plugineditor_unittest _source)
    ecm_add_test(${_source}
        NAME_PREFIX "messagecomposer-plugineditor-"
        LINK_LIBRARIES Qt5::Test KF5::MessageComposer KF5::XmlGui
    )
endmacro()

add_plugineditor_unittest(plugineditortest.cpp)

