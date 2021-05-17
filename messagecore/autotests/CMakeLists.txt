add_subdirectory(gnupg_home)

add_definitions( -DMAIL_DATA_DIR="${CMAKE_SOURCE_DIR}/mimetreeparser/autotests/data" )
add_definitions( -DDATA_DIR="${CMAKE_SOURCE_DIR}/messagecore/autotests/data" )

# Convenience macro to add unit tests.
macro(add_messagecore_test _source )
    add_definitions(-DKDESRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")
    ecm_add_test(${_source}
        NAME_PREFIX "messagecore-"
        LINK_LIBRARIES Qt::Test KF5::Libkleo KF5::MessageCore Qt::Widgets KF5::Archive KF5::Completion KF5::ConfigCore KF5::CoreAddons
    )
endmacro()

include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)

macro( add_messagecore_cryptotest _source )
    get_filename_component( _name ${_source} NAME_WE )
    add_executable( ${_name} ${_source} )
    target_link_libraries(
        ${_name}
        KF5::Archive
        KF5::Codecs
        KF5::Completion
        KF5::ConfigCore
        KF5::CoreAddons
        KF5::Libkleo
        KF5::MessageCore
        KF5::MimeTreeParser
        QGpgme
        Qt::Test
        Qt::Widgets
        )
    add_gpg_crypto_test(${_name} messagecore-${_name})
    set_tests_properties(messagecore-${_name} PROPERTIES RUN_SERIAL TRUE) # can't be parallelized due to gpg-agent
endmacro()

# Attachment tests.
add_messagecore_test( attachmentcompressjobtest.cpp )
add_messagecore_test( attachmentfrommimecontentjobtest.cpp )
add_messagecore_test( attachmentfromurljobtest.cpp )
add_messagecore_test( attachmentparttest.cpp )
add_messagecore_test( attachmentpropertiesdialogtest.cpp )
add_messagecore_test( attachmentupdatejobtest.cpp )

# Other tests
add_messagecore_test( stringutiltest.cpp )
add_messagecore_test( mailinglisttest.cpp )

# Autocrypt tests
add_messagecore_cryptotest( autocryptrecipienttest.cpp )
add_messagecore_cryptotest( autocryptstoragetest.cpp )
add_messagecore_cryptotest( autocryptutilstest.cpp )
