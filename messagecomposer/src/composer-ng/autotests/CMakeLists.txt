macro(add_composerng_autocorrect_unittest _source)
    ecm_add_test(${_source}
        NAME_PREFIX "messagecomposer-composerng-"
        LINK_LIBRARIES Qt5::Test KF5::MessageComposer
    )
endmacro()

add_composerng_autocorrect_unittest(richtextcomposerngtest.cpp)
