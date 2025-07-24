# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(-DMESSAGECORE_DATA_DIR="${CMAKE_SOURCE_DIR}/messagecore/autotests/data")
add_definitions(-DMAIL_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")
add_definitions(-DGNUPGHOME="${CMAKE_BINARY_DIR}/messagecore/autotests/gnupg_home")

include(${CMAKE_SOURCE_DIR}/cmake/modules/kdepim_add_gpg_crypto_test.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/part)

# Convenience macro to add messagecomposer unit tests.
macro(add_messagecomposer_test _source)
    get_filename_component(_name ${_source} NAME_WE)
    ecm_add_test(${ARGV}
        TEST_NAME ${_name}
        NAME_PREFIX "messagecomposer-"
        LINK_LIBRARIES
            KF6::I18n
            KPim6::MailTransport
            KPim6::Mime
            KPim6::MessageComposer
            KPim6::MessageCore
            KPim6::TemplateParser
            KPim6::PimTextEdit
            Qt::Test
            KPim6::IdentityManagementWidgets
            KF6::Contacts
            KPim6::AkonadiCore
            KF6::XmlGui
            KPim6::Libkdepim
            KF6::IconThemes
            KF6::Completion
)
endmacro()

macro(add_messagecomposer_cryptotest _source)
    set(_test cryptofunctions.cpp setupenv.cpp ${_source})
    get_filename_component(_name ${_source} NAME_WE)
    add_executable(${_name} ${_test}  ${_name}.h)
    target_link_libraries(
        ${_name}
        KF6::I18n
        KPim6::Mime
        KPim6::MessageComposer
        KPim6::MessageCore
        KPim6::Libkleo
        KPim6::Libkdepim
        KPim6::PimTextEdit
        KPim6::AkonadiCore
        Qt::Test
        KF6::IconThemes
        KF6::Completion
        KPim6::MailTransport
        KF6::Contacts
	KPim6::MimeTreeParser
	KF6::Service
)
    add_gpg_crypto_test(${_name} messagecomposer-${_name})
    set_tests_properties(messagecomposer-${_name} PROPERTIES
        ENVIRONMENT "LC_ALL=en_US.UTF-8"
)
endmacro()


# Utility stuff.
add_messagecomposer_test( draftstatustest.cpp)
add_messagecomposer_test( utiltest.cpp)
add_messagecomposer_test( messagefactoryngtest.cpp setupenv.cpp)
add_messagecomposer_test( plugineditorcheckbeforesendparamstest.cpp)
add_messagecomposer_test( replystrategytest.cpp)

# Non-content jobs.
add_messagecomposer_test( skeletonmessagejobtest.cpp)

# Basic content jobs.
add_messagecomposer_test( singlepartjobtest.cpp)
add_messagecomposer_test( multipartjobtest.cpp)

# More complex content jobs.
add_messagecomposer_test( attachmentjobtest.cpp)
add_messagecomposer_test( maintextjobtest.cpp)
add_messagecomposer_test( itipjobtest.cpp)

# Composer.
add_messagecomposer_test( composertest.cpp)
add_messagecomposer_cryptotest( cryptocomposertest.cpp)
add_messagecomposer_test( infoparttest.cpp)
add_messagecomposer_test( textparttest.cpp)
add_messagecomposer_test( globalparttest.cpp)
add_messagecomposer_cryptotest( composerviewbasetest.cpp)

add_messagecomposer_test( recipientseditortest.cpp)

# SendLater
add_messagecomposer_test( sendlaterdialogtest.cpp)
add_messagecomposer_test( sendlaterinfotest.cpp)

# Crypto
add_messagecomposer_cryptotest( autocryptheadersjobtest.cpp)
add_messagecomposer_cryptotest( signjobtest.cpp)
add_messagecomposer_cryptotest( encryptjobtest.cpp)
add_messagecomposer_cryptotest( signencrypttest.cpp)
add_messagecomposer_cryptotest( signandencrypttest.cpp)
add_messagecomposer_cryptotest( autocryptresolvercoretest.cpp)
add_messagecomposer_cryptotest( keyresolvertest.cpp)

set(AKONADI_RUN_POSTGRES_ISOLATED_TESTS FALSE)
set(AKONADI_RUN_MYSQL_ISOLATED_TESTS FALSE)

ecm_add_test(followupreminderselectdatedialogtest.cpp
    NAME_PREFIX "messagecomposer-"
    LINK_LIBRARIES
        KPim6::AkonadiCore
        KPim6::AkonadiWidgets
        KF6::CalendarCore
        KPim6::MessageComposer
        Qt::Test
)

ecm_add_test(attachmentvcardfromaddressbookjobtest.cpp
    NAME_PREFIX "messagecomposer-"
    LINK_LIBRARIES
        KF6::Contacts
        KPim6::MessageComposer
        Qt::Test
)
