cmake_minimum_required(VERSION 2.8.12)

project(Messagelib)

set(KF5_VERSION "5.25.0")
find_package(ECM ${KF5_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${Messagelib_SOURCE_DIR}/cmake/modules ${ECM_MODULE_PATH})
set(LIBRARY_NAMELINK)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGeneratePriFile)
include(ECMPackageConfigHelpers)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMQtDeclareLoggingCategory)
include(ECMAddTests)

set(PIM_VERSION "5.3.46")

set(MESSAGELIB_LIB_VERSION ${PIM_VERSION})
set(AKONADIMIME_LIB_VERSION "5.3.40")

set(QT_REQUIRED_VERSION "5.6.0")

set(AKONADI_VERSION "5.3.40")
set(AKONADINOTES_LIB_VERSION "5.3.40")
set(GRANTLEETHEME_LIB_VERSION "5.3.40")
set(GRAVATAR_LIB_VERSION "5.3.40")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.3.43")
set(KCONTACTS_LIB_VERSION "5.3.40")
set(KDEPIM_APPS_LIB_VERSION "5.3.40")
set(KLDAP_LIB_VERSION "5.3.40")
set(KMAILTRANSPORT_LIB_VERSION "5.3.40")
set(KMBOX_LIB_VERSION "5.3.40")
set(KMIME_LIB_VERSION "5.3.40")
set(KPIMTEXTEDIT_LIB_VERSION "5.3.40")
set(LIBKDEPIM_LIB_VERSION "5.3.41")
set(LIBKLEO_LIB_VERSION "5.3.40")
set(PIMCOMMON_LIB_VERSION "5.3.41")
=======
set(AKONADIMIME_LIB_VERSION "5.3.1")

set(QT_REQUIRED_VERSION "5.6.0")

set(AKONADI_VERSION "5.3.1")
set(AKONADINOTES_LIB_VERSION "5.3.1")
set(GRANTLEETHEME_LIB_VERSION "5.3.1")
set(GRAVATAR_LIB_VERSION "5.3.1")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.3.1")
set(KCONTACTS_LIB_VERSION "5.3.1")
set(KDEPIM_APPS_LIB_VERSION "5.3.1")
set(KLDAP_LIB_VERSION "5.3.1")
set(KMAILTRANSPORT_LIB_VERSION "5.3.1")
set(KMBOX_LIB_VERSION "5.3.1")
set(KMIME_LIB_VERSION "5.3.1")
set(KPIMTEXTEDIT_LIB_VERSION "5.3.1")
set(LIBKDEPIM_LIB_VERSION "5.3.1")
set(LIBKLEO_LIB_VERSION "5.3.1")
set(PIMCOMMON_LIB_VERSION "5.3.1")

find_package(KF5AkonadiSearch "5.3.40" CONFIG REQUIRED)
set_package_properties(KF5AkonadiSearch PROPERTIES DESCRIPTION "The Akonadi Search libraries" URL "http://www.kde.org" TYPE REQUIRED PURPOSE "Provides search capabilities in KMail and Akonadi")

option(KDEPIM_ENTERPRISE_BUILD "Enable features specific to the enterprise branch, which are normally disabled. Also, it disables many components not needed for Kontact such as the Kolab client." FALSE)


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
    string(REGEX REPLACE " [-0-9:+ ]*\n" "" messagelib_git_last_change "${messagelib_git_last_change}")
  endif()
endif()

add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

add_subdirectory(mimetreeparser)
add_subdirectory(messageviewer)
add_subdirectory(templateparser)
add_subdirectory(messagecomposer)
add_subdirectory(messagecore)
add_subdirectory(messagelist)
add_subdirectory(webengineviewer)

install( FILES messagelib.categories DESTINATION ${KDE_INSTALL_CONFDIR} )
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
