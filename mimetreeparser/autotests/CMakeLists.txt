set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)

# convenience macro to add qtest unit tests
macro(add_mimetreeparser_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} util.cpp setupenv.cpp testcsshelper.cpp
        TEST_NAME ${_name}
        NAME_PREFIX "mimetreeparser-"
        LINK_LIBRARIES KF5::MimeTreeParser Qt5::Test KF5::IconThemes
    )
endmacro ()

macro(add_mimetreeparser_class_unittest _source _additionalSource)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_additionalSource}
        TEST_NAME ${_name}
        NAME_PREFIX "mimetreeparser-"
        LINK_LIBRARIES KF5::MimeTreeParser Qt5::Test
    )
endmacro ()

macro(add_mimetreeparser_crypto_unittest _source)
    set(_test ${_source} util.cpp)
    get_filename_component(_name ${_source} NAME_WE)
    add_executable( ${_name} ${_test} setupenv.cpp  testcsshelper.cpp )
    ecm_mark_as_test(mimetreeparser-${_name})
    target_link_libraries( ${_name}
        KF5::MimeTreeParser
        Qt5::Test
	KF5::IconThemes
    )
    add_gpg_crypto_test(${_name} mimetreeparser-${_name})
endmacro ()

add_mimetreeparser_crypto_unittest(attachmenttest.cpp)
add_mimetreeparser_crypto_unittest(objecttreeparsertest.cpp)
add_mimetreeparser_crypto_unittest(rendertest.cpp)
add_mimetreeparser_crypto_unittest(unencryptedmessagetest.cpp)
add_mimetreeparser_unittest(messageparttest.cpp)
add_mimetreeparser_unittest(nodehelpertest.cpp)
add_mimetreeparser_unittest(csshelpertest.cpp)
add_mimetreeparser_class_unittest( cryptohelpertest.cpp "../src/viewer/cryptohelper.cpp")

ecm_add_test(converthtmltoplaintexttest.cpp ../src/viewer/converthtmltoplaintext.cpp
    TEST_NAME converthtmltoplaintexttest
    NAME_PREFIX "mimetreeparser-"
    LINK_LIBRARIES Qt5::Test Grantlee5::TextDocument
)
