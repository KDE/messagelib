cmake_minimum_required(VERSION 2.8.12)

project(Messagelib)

find_package(ECM 5.19.0 CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${Messagelib_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
set(LIBRARY_NAMELINK)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(ECMAddTests)

set(MESSAGELIB_LIB_VERSION "5.2.46")

set(KF5_VERSION "5.19.0")
set(KDEPIMLIBS_LIB_VERSION "5.2.40")

set(QT_REQUIRED_VERSION "5.4.0")
set(KMIME_LIB_VERSION "5.2.40")
set(KLDAP_LIB_VERSION "5.2.40")
set(KPIMTEXTEDIT_LIB_VERSION "5.2.40")
set(KMAILTRANSPORT_LIB_VERSION "5.2.40")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.2.40")
set(KCONTACTS_LIB_VERSION "5.2.40")
set(KMBOX_LIB_VERSION "5.2.40")
set(AKONADINOTES_LIB_VERSION "5.2.40")
set(KPIMTEXTEDIT_LIB_VERSION "5.2.40")
set(KDEPIM_APPS_LIB_VERSION "5.2.40")
set(LIBKDEPIM_LIB_VERSION "5.2.40")

set(AKONADI_VERSION "5.1.80")

find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Test UiTools Network WebKitWidgets)
find_package(KF5I18n ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiMime ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5SendLater ${KDEPIM_APPS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5FollowupReminder ${KDEPIM_APPS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkdepim ${LIBKDEPIM_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Archive ${KF5_VERSION} CONFIG REQUIRED)
find_package(Grantlee5 "5.0" CONFIG REQUIRED)
find_package(KF5Ldap ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimTextEdit ${KPIMTEXTEDIT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5WebKit ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5IconThemes ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Completion ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Codecs ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Config ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5XmlGui ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiSearch "5.0.42" CONFIG REQUIRED)
set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "http://www.kde.org" TYPE REQUIRED PURPOSE "Provides search capabilities in KMail and Akonadi")

find_package(KF5ItemViews ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Contacts ${KCONTACTS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Mbox ${KMBOX_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiNotes ${AKONADINOTES_LIB_VERSION} CONFIG REQUIRED)

set(GRAVATAR_LIB_VERSION "5.2.40")
set(GRANTLEETHEME_LIB_VERSION "5.2.40")
set(KDEPIM_APPS_LIBS_LIB_VERSION "5.2.40")
set(LIBKLEO_LIB_VERSION "5.2.40")
set(PIMCOMMON_LIB_VERSION "5.2.40")
find_package(Gpgme REQUIRED)
find_package(KF5KaddressbookGrantlee ${KDEPIM_APPS_LIBS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5GrantleeTheme ${GRANTLEETHEME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Gravatar ${GRAVATAR_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Libkleo ${LIBKLEO_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimCommon ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)

option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)

option(QTWEBENGINE_SUPPORT_OPTION "Enable support for QtWebEngine. False by default." FALSE)

option(MESSAGEVIEWER_USE_QTWEBENGINE "Use QtWebEngine in mail viewer. False by default." FALSE)

if (QTWEBENGINE_SUPPORT_OPTION)
    set(QT_REQUIRED_MINIMUM_WEBENGINE_VERSION "5.6")
    find_package(Qt5 ${QT_REQUIRED_MINIMUM_WEBENGINE_VERSION} CONFIG REQUIRED WebEngine WebEngineWidgets)
endif()


# Support for the GIT revision number in messagecomposer-version.h
if(EXISTS "${Messagelib_SOURCE_DIR}/.git")
  find_package(Git)
  if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                    WORKING_DIRECTORY ${Messagelib_SOURCE_DIR}
                    OUTPUT_VARIABLE messagelib_git_revision)
    string(REGEX REPLACE "\n" "" messagelib_git_revision "${messagelib_git_revision}")
    set(messagelib_git_revision "git-${messagelib_git_revision}")

    execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --oneline --format=%ci
                    WORKING_DIRECTORY ${Messagelib_SOURCE_DIR}
                    OUTPUT_VARIABLE messagelib_git_last_change)
    string(REGEX REPLACE " [0-9:+ ]*\n" "" messagelib_git_last_change "${messagelib_git_last_change}")
  endif()
endif()

add_subdirectory(mimetreeparser)
add_subdirectory(messageviewer)
add_subdirectory(templateparser)
add_subdirectory(messagecomposer)
add_subdirectory(messagecore)
add_subdirectory(messagelist)

install( FILES messagelib.categories DESTINATION ${KDE_INSTALL_CONFDIR} )
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
