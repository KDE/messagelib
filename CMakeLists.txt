# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "6.2.3")
set(CMAKE_CXX_STANDARD 20)

project(Messagelib VERSION ${PIM_VERSION})

option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)

set(KF_MIN_VERSION "6.3.0")
set(QT_REQUIRED_VERSION "6.6.0")

set(MESSAGELIB_LIB_VERSION ${PIM_VERSION})
set(AKONADIMIME_LIB_VERSION "6.2.3")

set(AKONADICONTACT_LIB_VERSION "6.2.3")
set(AKONADI_VERSION "6.2.3")
set(GRANTLEETHEME_LIB_VERSION "6.2.3")
set(GRAVATAR_LIB_VERSION "6.2.3")
set(IDENTITYMANAGEMENT_LIB_VERSION "6.2.3")
set(KMAILTRANSPORT_LIB_VERSION "6.2.3")
set(KMBOX_LIB_VERSION "6.2.3")
set(KMIME_LIB_VERSION "6.2.3")
set(KPIMTEXTEDIT_LIB_VERSION "6.2.3")
set(LIBKDEPIM_LIB_VERSION "6.2.3")
set(LIBKLEO_LIB_VERSION "6.2.3")
set(PIMCOMMON_LIB_VERSION "6.2.3")
set(GPGME_REQUIRED_VERSION "1.21.0")
set(AKONADI_SEARCH_VERSION "6.2.3")

set(ECM_VERSION ${KF_MIN_VERSION})

find_package(ECM ${ECM_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${Messagelib_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(ECMQtDeclareLoggingCategory)
include(ECMDeprecationSettings)
include(ECMFeatureSummary)
include(ECMAddTests)
include(ECMAddQch)
set(KTEXTADDONS_MIN_VERSION "1.5.4")

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

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
    add_definitions(-DHAVE_KTEXTADDONS_TEXT_TO_SPEECH_SUPPORT)
endif()

find_package(KF6TextAutoCorrectionWidgets ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)

find_package(KF6TextUtils ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6TextCustomEditor ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)
find_package(KF6TextAddonsWidgets ${KTEXTADDONS_MIN_VERSION} CONFIG REQUIRED)
find_package(KPim6AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG REQUIRED)
set_package_properties(KPim6AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "https://invent.kde.org/pim/akonadi-search" TYPE REQUIRED PURPOSE "Provides search capabilities in KMail and Akonadi")

ecm_set_disabled_deprecation_versions(QT 6.7.2 KF 6.4.0)
add_definitions(-DQT_NO_CONTEXTLESS_CONNECT)
if(BUILD_TESTING)
    add_definitions(-DBUILD_TESTING)
endif()

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

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
