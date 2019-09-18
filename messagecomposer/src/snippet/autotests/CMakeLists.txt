macro(add_composer_snippet_unittest _source)
    ecm_add_test(${_source}
        NAME_PREFIX "messagecomposer-snippet-"
        LINK_LIBRARIES Qt5::Test KF5::MessageComposer
    )
endmacro()

#add_composer_snippet_unittest(composerattachmentinterfacetest.cpp)

