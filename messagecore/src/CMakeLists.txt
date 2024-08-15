# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGECORE
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messagecore_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageCoreConfigVersion.cmake"
    SOVERSION 6
)

add_definitions(-DTRANSLATION_DOMAIN=\"libmessagecore6\")

add_library(KPim6MessageCore)
add_library(KPim6::MessageCore ALIAS KPim6MessageCore)

########### next target ###############
target_sources(KPim6MessageCore PRIVATE
    autocrypt/autocryptutils.cpp
    autocrypt/autocryptrecipient.cpp
    autocrypt/autocryptstorage.cpp

    attachment/attachmentcompressjob.cpp
    attachment/attachmentfromfolderjob.cpp
    attachment/attachmentfrommimecontentjob.cpp
    attachment/attachmentfromurlbasejob.cpp
    attachment/attachmentfromurljob.cpp
    attachment/attachmentloadjob.cpp
    attachment/attachmentpart.cpp
    attachment/attachmentpropertiesdialog.cpp
    attachment/attachmentupdatejob.cpp
    attachment/attachmentfromurlutils.cpp

    misc/imagecollector.cpp
    misc/mailinglist.cpp

    utils/stringutil.cpp
    utils/util.cpp
    utils/colorutil.cpp
    utils/dateformatter.cpp

    settings/messagecoresettings.cpp
    misc/imagecollector.h
    misc/mailinglist.h
    utils/colorutil.h
    utils/dateformatter.h
    utils/stringutil.h
    utils/util.h
    attachment/attachmentloadjob.h
    attachment/attachmentfrommimecontentjob.h
    attachment/attachmentfromfolderjob.h
    attachment/attachmentfromurlbasejob.h
    attachment/attachmentcompressjob.h
    attachment/attachmentpart.h
    attachment/attachmentfromurljob.h
    attachment/attachmentfromurlutils.h
    attachment/attachmentpropertiesdialog.h
    attachment/attachmentupdatejob.h
    messagecore_private_export.h
    autocrypt/autocryptutils.h
    autocrypt/autocryptstorage.h
    autocrypt/autocryptstorage_p.h
    autocrypt/autocryptrecipient.h
    autocrypt/autocryptrecipient_p.h
    settings/messagecoresettings.h
)

kconfig_add_kcfg_files(KPim6MessageCore
    settings/globalsettings_messagecore.kcfgc
)

ki18n_wrap_ui(KPim6MessageCore
    attachment/ui/attachmentpropertiesdialog.ui
    attachment/ui/attachmentpropertiesdialog_readonly.ui
)

ecm_qt_declare_logging_category(KPim6MessageCore HEADER messagecore_debug.h IDENTIFIER MESSAGECORE_LOG CATEGORY_NAME org.kde.pim.messagecore
    DESCRIPTION "messagelib (messagecore)"
    OLD_CATEGORY_NAMES log_messagecore
    EXPORT MESSAGELIB
)

ecm_qt_declare_logging_category(KPim6MessageCore HEADER autocrypt_debug.h IDENTIFIER AUTOCRYPT_LOG CATEGORY_NAME org.kde.pim.messagecore.autocrypt
    DESCRIPTION "messagelib (messagecore.autocrypt)"
    EXPORT MESSAGELIB
)

if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim6MessageCore PROPERTIES UNITY_BUILD ON)
endif()

ecm_generate_export_header(KPim6MessageCore
    BASE_NAME messagecore
    VERSION ${MESSAGECORE_VERSION}
    DEPRECATED_BASE_VERSION 0
    USE_VERSION_HEADER
)

target_link_libraries(KPim6MessageCore
    PUBLIC
    KPim6::Mime
    PRIVATE
    KF6::KIOCore
    KF6::Codecs
    KF6::Archive
    Qt::Network
    KF6::I18n
    KPim6::IdentityManagementCore
    KPim6::MimeTreeParser
    KF6::ColorScheme
    KF6::ConfigGui
    Qt::Widgets
    KPim6::PimTextEdit
)
target_link_libraries(KPim6MessageCore PRIVATE QGpgmeQt6)

target_include_directories(KPim6MessageCore INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageCore/>")

set_target_properties(KPim6MessageCore PROPERTIES
    VERSION ${MESSAGECORE_VERSION}
    SOVERSION ${MESSAGECORE_SOVERSION}
    EXPORT_NAME MessageCore
)

install(TARGETS
    KPim6MessageCore
    EXPORT KPim6MessageCoreTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)

ecm_generate_headers(MessageCore_Camelcasemisc_HEADERS
    HEADER_NAMES
    ImageCollector
    MailingList

    REQUIRED_HEADERS MessageCore_misc_HEADERS
    PREFIX MessageCore
    RELATIVE misc
)

ecm_generate_headers(MessageCore_Camelcaseutils_HEADERS
    HEADER_NAMES
    ColorUtil
    StringUtil
    Util
    DateFormatter
    REQUIRED_HEADERS MessageCore_utils_HEADERS
    PREFIX MessageCore
    RELATIVE utils
)

ecm_generate_headers(MessageCore_Camelcasesettings_HEADERS
    HEADER_NAMES
    MessageCoreSettings
    REQUIRED_HEADERS MessageCore_settings_HEADERS
    PREFIX MessageCore
    RELATIVE settings
)


ecm_generate_headers(MessageCore_Camelcaseattachment_HEADERS
    HEADER_NAMES
    AttachmentFromUrlBaseJob
    AttachmentPart
    AttachmentPropertiesDialog
    AttachmentCompressJob
    AttachmentUpdateJob
    AttachmentFromUrlUtils
    AttachmentLoadJob

    REQUIRED_HEADERS MessageCore_attachment_HEADERS
    PREFIX MessageCore
    RELATIVE attachment
)

ecm_generate_headers(MessageCore_Camelcaseautocrypt_HEADERS
    HEADER_NAMES
    AutocryptRecipient
    AutocryptStorage
    AutocryptUtils
    REQUIRED_HEADERS MessageCore_autocrypt_HEADERS
    PREFIX MessageCore
    RELATIVE autocrypt
)

install(FILES
    ${MessageCore_Camelcaseautocrypt_HEADERS}
    ${MessageCore_Camelcasesettings_HEADERS}
    ${MessageCore_Camelcaseutils_HEADERS}
    ${MessageCore_Camelcaseattachment_HEADERS}
    ${MessageCore_Camelcasemisc_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageCore/MessageCore
    COMPONENT Devel
)

install(FILES
    ${MessageCore_autocrypt_HEADERS}
    ${MessageCore_settings_HEADERS}
    ${MessageCore_utils_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messagecore_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/globalsettings_messagecore.h
    ${MessageCore_attachment_HEADERS}
    ${MessageCore_misc_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageCore/messagecore
    COMPONENT Devel
)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MessageCore")


if(BUILD_QCH)
    ecm_add_qch(
        KPim6MessageCore_QCH
        NAME KPim6MessageCore
        BASE_NAME KPim6MessageCore
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${MessageCore_autocrypt_HEADERS}
        ${MessageCore_settings_HEADERS}
        ${MessageCore_utils_HEADERS}
        LINK_QCHS
        Qt6Core_QCH
        Qt6Gui_QCH
        Qt6Widgets_QCH
        INCLUDE_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
        MESSAGECORE_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
)

ecm_install_qch_export(
    TARGETS KPim6MessageCore_QCH
    FILE KPim6MessageCoreQchTargets.cmake
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)
set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim6MessageCoreQchTargets.cmake\")")
endif()

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMessageCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageCoreConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageCoreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageCoreConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT KPim6MessageCoreTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MessageCoreTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messagecore_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageCore COMPONENT Devel
)

