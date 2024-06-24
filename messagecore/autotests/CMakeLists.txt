# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_subdirectory(gnupg_home)

add_definitions(-DMAIL_DATA_DIR="${CMAKE_SOURCE_DIR}/mimetreeparser/autotests/data")
add_definitions(-DDATA_DIR="${CMAKE_SOURCE_DIR}/messagecore/autotests/data")

# Convenience macro to add unit tests.
macro(add_messagecore_test _source)
    add_definitions(-DKDESRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")
    set(_test ${_source})
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(${_name} ${_test} ${_name}.h)
    add_test(NAME ${_name} COMMAND ${_name})
    ecm_mark_as_test(${_name})
    target_link_libraries(${_name} Qt::Test KPim6::Libkleo KPim6::MessageCore Qt::Widgets KF6::Archive KF6::Completion KF6::ConfigCore KF6::CoreAddons)
endmacro()


include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)

macro(add_messagecore_cryptotest _source)
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(${_name} ${_source} ${_name}.h)
    target_link_libraries(
        ${_name}
        KF6::Archive
        KF6::Codecs
        KF6::Completion
        KF6::ConfigCore
        KF6::CoreAddons
        KPim6::Libkleo
        KPim6::MessageCore
        KPim6::MimeTreeParser
        Qt::Test
        Qt::Widgets
)
    add_gpg_crypto_test(${_name} messagecore-${_name})
endmacro()

# Attachment tests.
add_messagecore_test( attachmentcompressjobtest.cpp)
add_messagecore_test( attachmentfrommimecontentjobtest.cpp)
add_messagecore_test( attachmentfromurljobtest.cpp)
add_messagecore_test( attachmentparttest.cpp)
add_messagecore_test( attachmentpropertiesdialogtest.cpp)
add_messagecore_test( attachmentupdatejobtest.cpp)

# Other tests
add_messagecore_test( stringutiltest.cpp)
add_messagecore_test( mailinglisttest.cpp)
ecm_add_test(dateformattertest.cpp LINK_LIBRARIES Qt::Test KPim6::MessageCore)

# Autocrypt tests
add_messagecore_cryptotest( autocryptrecipienttest.cpp)
add_messagecore_cryptotest( autocryptstoragetest.cpp)
add_messagecore_cryptotest( autocryptutilstest.cpp)
