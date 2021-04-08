macro(add_composer_unittest _source)
    ecm_add_test(${_source}
        NAME_PREFIX "messagecomposer-composer-"
        LINK_LIBRARIES Qt::Test KF5::MessageComposer
    )
endmacro()

add_composer_unittest(composerattachmentinterfacetest.cpp)

