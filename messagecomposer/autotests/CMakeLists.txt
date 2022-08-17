# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions( -DMESSAGECORE_DATA_DIR="${CMAKE_SOURCE_DIR}/messagecore/autotests/data" )
add_definitions( -DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data" )
add_definitions( -DGNUPGHOME="${CMAKE_BINARY_DIR}/messagecore/autotests/gnupg_home")

include( ${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/part)

# Convenience macro to add messagecomposer unit tests.
macro( add_messagecomposer_test _source )
    get_filename_component( _name ${_source} NAME_WE )
    ecm_add_test(${ARGV}
        TEST_NAME ${_name}
        NAME_PREFIX "messagecomposer-"
        LINK_LIBRARIES
            KF5::I18n
            KF5::MailTransport
            KF5::Mime
            KF5::MessageComposer
            KF5::MessageViewer
            KF5::MessageCore
            KF5::TemplateParser
            KF5::PimTextEdit
            Qt::Test
            KF5::IdentityManagementWidgets
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
    add_executable( ${_name} ${_test}  ${_name}.h )
    target_link_libraries(
        ${_name}
        KF5::I18n
        KF5::Mime
        KF5::MessageComposer
        KF5::MessageViewer
        KF5::MessageCore
        KF5::Libkleo
        KF5::Libkdepim
        KF5::PimTextEdit
        KF5::AkonadiCore
        Qt::Test
        KF5::IconThemes
        KF5::Completion
        KF5::MailTransport
        KF5::Contacts
    )
    add_gpg_crypto_test(${_name} messagecomposer-${_name})
    set_tests_properties(messagecomposer-${_name} PROPERTIES
        ENVIRONMENT "LC_ALL=en_US.UTF-8"
    )
endmacro()


# Utility stuff.
add_messagecomposer_test( draftstatustest.cpp )
add_messagecomposer_test( utiltest.cpp )
add_messagecomposer_test( messagefactoryngtest.cpp setupenv.cpp)
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
add_messagecomposer_cryptotest( composerviewbasetest.cpp )

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
add_messagecomposer_cryptotest( keyresolvertest.cpp )

ecm_add_test( nearexpirycheckertest.cpp setupenv.cpp
        TEST_NAME "nearexpirycheckertest"
        NAME_PREFIX "messagecomposer-"
        LINK_LIBRARIES
            KF5::MessageComposer
            Qt::Test
            KF5::Libkleo
    )

set(AKONADI_RUN_POSTGRES_ISOLATED_TESTS FALSE)
set(AKONADI_RUN_MYSQL_ISOLATED_TESTS FALSE)

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
