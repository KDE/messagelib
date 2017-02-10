set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DMAIL_DATA_DIR="${CMAKE_SOURCE_DIR}/mimetreeparser/autotests/data" )

include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)

macro(add_messageviewer_crypto_unittest _source)
    set(_test ${_source} util.cpp)
    get_filename_component(_name ${_source} NAME_WE)
    add_executable( ${_name} ${_test} setupenv.cpp  testcsshelper.cpp )
    ecm_mark_as_test(messageviewer-messageparttheme-${_name})
    target_link_libraries( ${_name}
        KF5::MessageViewer
        Qt5::Test
        KF5::IconThemes
        KF5::Mime
        KF5::GrantleeTheme
    )
    add_gpg_crypto_test(${_name} messageviewer-messageparttheme-${_name})
endmacro ()

add_messageviewer_crypto_unittest(objecttreeparsertest.cpp)
add_messageviewer_crypto_unittest(rendertest.cpp)
add_messageviewer_crypto_unittest(unencryptedmessagetest.cpp)

ecm_add_test(quotehtmltest.cpp setupenv.cpp ../defaultrenderer.cpp ../htmlblock.cpp ../converthtmltoplaintext.cpp ../../../messageviewer_debug.cpp
    TEST_NAME quotehtmltest
    NAME_PREFIX "messageviewer-messageparttheme-"
    LINK_LIBRARIES Qt5::Test KF5::GrantleeTheme KF5::MessageViewer KF5::MimeTreeParser KF5::MessageCore KF5::IconThemes KF5::Codecs KF5::I18n Grantlee5::TextDocument Grantlee5::Templates
)

ecm_add_test(converthtmltoplaintexttest.cpp ../converthtmltoplaintext.cpp
    TEST_NAME converthtmltoplaintexttest
    NAME_PREFIX "messageviewer-messageparttheme-"
    LINK_LIBRARIES Qt5::Test Grantlee5::TextDocument
)
