cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(PIM_VERSION "5.19.3")


project(Messagelib VERSION ${PIM_VERSION})

option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)

option(MESSAGEVIEWER_EXPERIMENTAL_CONVERSATIONVIEW "Experimental conversationview (in progress)" FALSE)

set(KF5_MIN_VERSION "5.87.0")


set(MESSAGELIB_LIB_VERSION ${PIM_VERSION})
set(AKONADIMIME_LIB_VERSION "5.19.2")

set(QT_REQUIRED_VERSION "5.15.2")
set(AKONADICONTACT_LIB_VERSION "5.19.2")
set(AKONADI_VERSION "5.19.2")
set(GRANTLEETHEME_LIB_VERSION "5.19.2")
set(GRAVATAR_LIB_VERSION "5.19.2")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.19.2")
set(KMAILTRANSPORT_LIB_VERSION "5.19.2")
set(KMBOX_LIB_VERSION "5.19.2")
set(KMIME_LIB_VERSION "5.19.2")
set(KPIMTEXTEDIT_LIB_VERSION "5.19.2")
set(LIBKDEPIM_LIB_VERSION "5.19.2")
set(LIBKLEO_LIB_VERSION "5.19.2")
set(PIMCOMMON_LIB_VERSION "5.19.2")
set(GPGME_LIB_VERSION "1.13.1")
set(AKONADI_SEARCH_VERSION "5.19.2")

set(ECM_VERSION ${KF5_MIN_VERSION})

find_package(ECM ${ECM_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${Messagelib_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)

include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

include(ECMQtDeclareLoggingCategory)
include(ECMAddTests)
include(ECMAddQch)


option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")


find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Gui Test)

find_package(KF5Codecs ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5I18n ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5NewStuff ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(QGpgme ${GPGME_LIB_VERSION} CONFIG REQUIRED)
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Network PrintSupport WebEngine WebEngineWidgets)
find_package(KF5Archive ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Completion ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Config ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5ConfigWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5IconThemes ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5ItemViews ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5JobWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5KIO ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Service ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Sonnet ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5TextWidgets ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5WidgetsAddons ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5XmlGui ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5SyntaxHighlighting ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5GuiAddons ${KF5_MIN_VERSION} CONFIG REQUIRED)

find_package(Grantlee5 "5.2" CONFIG REQUIRED) #Use KF5_VERSION in the future

find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiMime ${AKONADIMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Contacts ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiContact ${AKONADICONTACT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5GrantleeTheme ${GRANTLEETHEME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Gravatar ${GRAVATAR_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MailTransportAkonadi ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Mbox ${KMBOX_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimCommonAkonadi ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimTextEdit ${KPIMTEXTEDIT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)

find_package(KF5AkonadiSearch ${AKONADI_SEARCH_VERSION} CONFIG REQUIRED)
set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "https://invent.kde.org/pim/akonadi-search" TYPE REQUIRED PURPOSE "Provides search capabilities in KMail and Akonadi")

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050e00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055800)
if(BUILD_TESTING)
    add_definitions(-DBUILD_TESTING)
endif()

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" OFF)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT OFF)
if (USE_UNITY_CMAKE_SUPPORT)
    set(COMPILE_WITH_UNITY_CMAKE_SUPPORT ON)
    add_definitions(-DCOMPILE_WITH_UNITY_CMAKE_SUPPORT)
endif()

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
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
