# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DMAIL_DATA_DIR="${CMAKE_SOURCE_DIR}/mimetreeparser/autotests/data")

include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)

macro(add_messageviewer_crypto_unittest _source)
    set(_test ${_source} util.cpp)
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(${_name} ${_test} setupenv.cpp  testcsshelper.cpp)
    ecm_mark_as_test(messageviewer-messageparttheme-${_name})
    target_link_libraries(${_name}
        KPim6::MessageViewer
        Qt::Test
        KF6::IconThemes
        KPim6::Mime
        KPim6::GrantleeTheme
)
    add_gpg_crypto_test(${_name} messageviewer-messageparttheme-${_name})
    set_tests_properties(messageviewer-messageparttheme-${_name} PROPERTIES
        ENVIRONMENT "LC_ALL=en_US.UTF-8"
)
endmacro()

add_messageviewer_crypto_unittest(objecttreeparsertest.cpp)
add_messageviewer_crypto_unittest(rendertest.cpp)
add_messageviewer_crypto_unittest(unencryptedmessagetest.cpp)

ecm_add_test(quotehtmltest.cpp setupenv.cpp ../plugins/quotehtml.cpp
    TEST_NAME quotehtmltest
    NAME_PREFIX "messageviewer-messageparttheme-"
    LINK_LIBRARIES Qt::Test KPim6::MessageViewer KF6::IconThemes KPim6::Mime
)

ecm_add_test(showonlymimeparttest.cpp setupenv.cpp util.cpp testcsshelper.cpp
    TEST_NAME showonlymimeparttest
    NAME_PREFIX "messageviewer-messageparttheme-"
    LINK_LIBRARIES Qt::Test KPim6::MessageViewer KF6::IconThemes KPim6::Mime KPim6::GrantleeTheme
)

ecm_add_test(converthtmltoplaintexttest.cpp ../converthtmltoplaintext.cpp
    TEST_NAME converthtmltoplaintexttest
    NAME_PREFIX "messageviewer-messageparttheme-"
    LINK_LIBRARIES Qt::Test KPim6::PimTextEdit Qt::Gui
)
