set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )

include( ${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/part)

# Convenience macro to add messagecomposer unit tests.
macro( add_messagecomposer_test _source )
    ecm_add_test(${_source}
        NAME_PREFIX "messagecomposer-"
        LINK_LIBRARIES 
            KF5::Mime
            KF5::MessageComposer
            KF5::MessageViewer
            KF5::MessageCore
            KF5::TemplateParser
            KF5::PimTextEdit
            Qt::Test
            KF5::IdentityManagement
            KF5::Contacts
            KF5::AkonadiCore
            KF5::XmlGui
            KF5::Libkdepim
            KF5::IconThemes
            KF5::Completion
    )
endmacro()

macro( add_messagecomposer_cryptotest _source )
    set( _test cryptofunctions.cpp setupenv.cpp ${_source} )
    get_filename_component( _name ${_source} NAME_WE )
    add_executable( ${_name} ${_test} )
    target_link_libraries(
        ${_name}
        KF5::Mime
        KF5::MessageComposer
        KF5::MessageViewer
        KF5::MessageCore
        KF5::Libkleo
        KF5::Libkdepim
        KF5::PimTextEdit
        KF5::AkonadiCore
        Qt::Test
        QGpgme
        KF5::IconThemes
        KF5::Completion
        KF5::MailTransport
        KF5::Ldap
        KF5::Contacts
    )
    add_gpg_crypto_test(${_name} messagecomposer-${_name})
    set_tests_properties(messagecomposer-${_name} PROPERTIES RUN_SERIAL TRUE) # can't be parallelized due to gpg-agent
endmacro()


# Utility stuff.
add_messagecomposer_test( utiltest.cpp )
add_messagecomposer_test( messagefactoryngtest.cpp )
add_messagecomposer_test( plugineditorcheckbeforesendparamstest.cpp )
add_messagecomposer_test( replystrategytest.cpp )

# Non-content jobs.
add_messagecomposer_test( skeletonmessagejobtest.cpp )

# Basic content jobs.
add_messagecomposer_test( singlepartjobtest.cpp )
add_messagecomposer_test( multipartjobtest.cpp )

# More complex content jobs.
add_messagecomposer_test( attachmentjobtest.cpp )
add_messagecomposer_test( maintextjobtest.cpp )

# Composer.
add_messagecomposer_test( composertest.cpp )
add_messagecomposer_cryptotest( cryptocomposertest.cpp )
add_messagecomposer_test( infoparttest.cpp )
add_messagecomposer_test( textparttest.cpp )
add_messagecomposer_test( globalparttest.cpp )
add_messagecomposer_test( composerviewbasetest.cpp )

add_messagecomposer_test( recipientseditortest.cpp )

# SendLater
add_messagecomposer_test( sendlaterdialogtest.cpp )
add_messagecomposer_test( sendlaterinfotest.cpp )

# Crypto
add_messagecomposer_cryptotest( autocryptheadersjobtest.cpp )
add_messagecomposer_cryptotest( signjobtest.cpp )
add_messagecomposer_cryptotest( encryptjobtest.cpp )
add_messagecomposer_cryptotest( signencrypttest.cpp )
add_messagecomposer_cryptotest( signandencrypttest.cpp )

if (KDEPIM_RUN_AKONADI_TEST)
    set(AKONADI_RUN_POSTGRES_ISOLATED_TESTS FALSE)
    set(AKONADI_RUN_MYSQL_ISOLATED_TESTS FALSE)

    add_executable( keyresolvertest
        cryptofunctions.cpp
        setupenv.cpp
        keyresolvertest.cpp
    )
    target_link_libraries( keyresolvertest
            KF5::AkonadiCore
            KF5::MessageComposer
            Qt::Test
    )
    add_gpg_crypto_akonadi_test(keyresolvertest messagecomposer-keyresolvertest)
    if (NOT DEFINED AKONADI_RUN_SQLITE_ISOLATED_TESTS OR AKONADI_RUN_SQLITE_ISOLATED_TESTS)
        set_tests_properties(akonadi-sqlite-messagecomposer-keyresolvertest PROPERTIES RUN_SERIAL TRUE) # can't be parallelized due to gpg-agent
    endif()
endif()

ecm_add_test(followupreminderselectdatedialogtest.cpp
    NAME_PREFIX "messagecomposer-"
    LINK_LIBRARIES
        KF5::AkonadiCore
        KF5::AkonadiWidgets
        KF5::CalendarCore
        KF5::MessageComposer
        Qt::Test
)

ecm_add_test(attachmentvcardfromaddressbookjobtest.cpp
    NAME_PREFIX "messagecomposer-"
    LINK_LIBRARIES
        KF5::Contacts
        KF5::MessageComposer
        Qt::Test
)
