# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
cmake_minimum_required(VERSION 3.27)
set(PIM_VERSION "6.6.44")
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

project(Messagelib VERSION ${PIM_VERSION})

option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)

set(KF_MIN_VERSION "6.20.0")
set(QT_REQUIRED_VERSION "6.9.0")

set(MESSAGELIB_LIB_VERSION ${PIM_VERSION})
set(AKONADIMIME_LIB_VERSION "6.6.40")

set(AKONADICONTACT_LIB_VERSION "6.6.40")
set(AKONADI_VERSION "6.6.40")
set(GRANTLEETHEME_LIB_VERSION "6.6.40")
set(GRAVATAR_LIB_VERSION "6.6.40")
set(IDENTITYMANAGEMENT_LIB_VERSION "6.6.40")
set(KMAILTRANSPORT_LIB_VERSION "6.6.40")
set(KMBOX_LIB_VERSION "6.6.40")
set(KMIME_LIB_VERSION "6.6.45")
set(KPIMTEXTEDIT_LIB_VERSION "6.6.40")
set(LIBKDEPIM_LIB_VERSION "6.6.40")
set(LIBKLEO_LIB_VERSION "6.6.40")
set(PIMCOMMON_LIB_VERSION "6.6.41")
set(GPGME_REQUIRED_VERSION "1.23.2")
set(AKONADI_SEARCH_VERSION "6.6.40")
set(MIMETREEPARSER_LIB_VERSION "6.6.40")

set(OPENSSL_VERSION "3.0.0")

set(ECM_MIN_VERSION ${KF_MIN_VERSION})

find_package(ECM ${ECM_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${Messagelib_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

# setup use of ECMAddTest copy with newer features
if(ECM_VERSION VERSION_LESS 6.22)
    list(PREPEND CMAKE_MODULE_PATH "${Messagelib_SOURCE_DIR}/cmake/ecmcopy")
elseif(ECM_MIN_VERSION VERSION_GREATER_EQUAL 6.22)
    message(WARNING "Remove copy of ECMAddTest, min. required ECM now provides it.")
endif()

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES
messagecomposer/*.cpp
messagecomposer/*.h
messagecore/*.cpp
messagecore/*.h
messagelist/*.cpp
messagelist/*.h
messageviewer/*.cpp
messageviewer/*.h
mimetreeparser/*.cpp
mimetreeparser/*.h
templateparser/*.cpp
templateparser/*.h
webengineviewer/*.cpp
webengineviewer/*.h
)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMFeatureSummary)
include(ECMAddTests)
include(ECMGenerateQDoc)

include(ECMCheckOutboundLicense)
file(GLOB_RECURSE ALL_SOURCE_FILES
messagecomposer/*.cpp
messagecomposer/*.h
messagecore/*.cpp
messagecore/*.h
messagelist/*.cpp
messagelist/*.h
messageviewer/*.cpp
messageviewer/*.h
mimetreeparser/*.cpp
mimetreeparser/*.h
templateparser/*.cpp
templateparser/*.h
webengineviewer/*.cpp
webengineviewer/*.h
)
ecm_check_outbound_license(LICENSES GPL-2.0-only  FILES ${ALL_SOURCE_FILES})


set(KTEXTADDONS_MIN_VERSION "1.9.0")

find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Gui Test)
find_package(Qt6 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Network PrintSupport WebEngineWidgets)

find_package(KF6CoreAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Codecs ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6I18n ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6NewStuff ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(Gpgmepp ${GPGME_REQUIRED_VERSION} CONFIG REQUIRED)
find_package(QGpgmeQt6 ${GPGME_REQUIRED_VERSION} CONFIG REQUIRED)
find_package(KF6Archive ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Completion ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Config ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Contacts ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6IconThemes ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6ItemViews ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6JobWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6KIO ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Service ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Sonnet ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6TextWidgets ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6WidgetsAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6XmlGui ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6SyntaxHighlighting ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6GuiAddons ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6Notifications ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6ColorScheme ${KF_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6TextTemplate CONFIG REQUIRED)

find_package(KF6CalendarCore ${KF_MIN_VERSION} CONFIG REQUIRED)

find_package(KPim6Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KPim6AkonadiMime ${AKONADIMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6AkonadiContactWidgets ${AKONADICONTACT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6GrantleeTheme ${GRANTLEETHEME_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6Gravatar ${GRAVATAR_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6IdentityManagementWidgets ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6Libkleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6Mbox ${KMBOX_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6PimCommonAkonadi ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6TextEdit ${KPIMTEXTEDIT_LIB_VERSION} CONFIG REQUIRED)
find_package(KPim6Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)

find_package(KF6TextEditTextToSpeech ${KTEXTADDONS_MIN_VERSION} CONFIG)
if(TARGET KF6::TextEditTextToSpeech)
    set(HAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT TRUE)
endif()

find_package(KF6TextAddonsWidgets ${KTEXTADDONS_MIN_VERSION} CONFIG)

find_package(KF6TextAutoCorrectionWidgets ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)

find_package(KF6TextUtils ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6TextCustomEditor ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6TextAddonsWidgets ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)

option(FORCE_DISABLE_AKONADI_SEARCH "Disable features and API that require akonadi-search, e.g. because xapian isn't available" OFF)
if(UNIX)
    if(NOT FORCE_DISABLE_AKONADI_SEARCH)
        find_package(KPim6AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG REQUIRED)
        set_package_properties(KPim6AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "https://invent.kde.org/pim/akonadi-search" TYPE REQUIRED PURPOSE "Provides search capabilities in KMail and Akonadi")
    endif()
else()
    if(NOT FORCE_DISABLE_AKONADI_SEARCH)
        find_package(KPim6AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG)
        set_package_properties(KPim6AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "https://invent.kde.org/pim/akonadi-search" TYPE OPTIONAL PURPOSE "Provides search capabilities in KMail and Akonadi")
    endif()
endif()

if(NOT KPim6AkonadiSearch_FOUND)
    set(FORCE_DISABLE_AKONADI_SEARCH TRUE)
endif()

find_package(OpenSSL ${OPENSSL_VERSION} REQUIRED)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Not setting for GNU due to too many warnings related to private members of base classes or around lambdas
    # see e.g. https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56556 or https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79328
    string(APPEND CMAKE_CXX_FLAGS " -Wshadow")
endif()
ecm_set_disabled_deprecation_versions(QT 6.10.0 KF 6.21.0 KMIME 6.7.0)
add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
if(BUILD_TESTING)
    set(ECM_TEST_NAME_PREFIX_STRATEGY "PATH")
    add_definitions(-DBUILD_TESTING)
endif()

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

option(ENABLE_PCH "Enable Precompiled Headers support" ON)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if(USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
    add_definitions(-DCOMPILE_WITH_UNITY_CMAKE_SUPPORT)
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(mimetreeparser)
add_subdirectory(messageviewer)
add_subdirectory(templateparser)
add_subdirectory(messagecomposer)
add_subdirectory(messagecore)
add_subdirectory(messagelist)
add_subdirectory(webengineviewer)

ecm_qt_install_logging_categories(
    EXPORT MESSAGELIB
    FILE messagelib.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
