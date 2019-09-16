macro(add_composer_unittest _source)
    ecm_add_test(${_source}
        NAME_PREFIX "messagecomposer-composer-"
        LINK_LIBRARIES Qt5::Test KF5::MessageComposer
    )
endmacro()

add_plugineditor_unittest(composerattachmentinterfacetest.cpp)

