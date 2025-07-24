# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_definitions(-DSIGNATURE_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

macro(add_composerng_autocorrect_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_name}.h
        TEST_NAME ${_name}
        NAME_PREFIX "messagecomposer-composerng-"
        LINK_LIBRARIES Qt::Test KPim6::MessageComposer KF6::XmlGui
)
endmacro()

add_composerng_autocorrect_unittest(richtextcomposerngtest.cpp)
add_composerng_autocorrect_unittest(richtextcomposersignaturestest.cpp)
