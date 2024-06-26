# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)

# convenience macro to add qtest unit tests
macro(add_mimetreeparser_unittest _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} util.cpp setupenv.cpp
        TEST_NAME ${_name}
        NAME_PREFIX "mimetreeparser-"
        LINK_LIBRARIES KPim6::MimeTreeParser Qt::Test KPim6::Mime Gpgmepp
)
endmacro()

macro(add_mimetreeparser_class_unittest _source _additionalSource)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${_source} ${_additionalSource}
        TEST_NAME ${_name}
        NAME_PREFIX "mimetreeparser-"
        LINK_LIBRARIES KPim6::MimeTreeParser Qt::Test KPim6::Mime Gpgmepp
)
endmacro()

macro(add_mimetreeparser_crypto_unittest _source)
    set(_test ${_source} util.cpp)
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(${_name} ${_test} setupenv.cpp)
    ecm_mark_as_test(mimetreeparser-${_name})
    target_link_libraries(${_name}
        KPim6::MimeTreeParser
        Qt::Test
        KPim6::Mime
        QGpgmeQt6
)
    add_gpg_crypto_test(${_name} mimetreeparser-${_name})
endmacro()

add_mimetreeparser_crypto_unittest(attachmenttest.cpp)
add_mimetreeparser_crypto_unittest(basicobjecttreeparsertest.cpp)
add_mimetreeparser_crypto_unittest(protectedheaderstest.cpp)
add_mimetreeparser_unittest(bodypartformatterbasefactorytest.cpp)
add_mimetreeparser_unittest(nodehelpertest.cpp)
add_mimetreeparser_class_unittest(cryptohelpertest.cpp "../src/cryptohelper.cpp")
