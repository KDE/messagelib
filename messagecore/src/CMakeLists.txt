# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_definitions(-DTRANSLATION_DOMAIN=\"libmessagecore\")

add_library(KF5MessageCore)
add_library(KF5::MessageCore ALIAS KF5MessageCore)

########### next target ###############
target_sources(KF5MessageCore PRIVATE
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

    helpers/nodehelper.cpp

    utils/stringutil.cpp
    utils/util.cpp
    utils/colorutil.cpp

    settings/messagecoresettings.cpp
    misc/imagecollector.h
    misc/mailinglist.h
    utils/colorutil.h
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
    helpers/nodehelper.h
    settings/messagecoresettings.h
    )

kconfig_add_kcfg_files(KF5MessageCore
    settings/globalsettings_messagecore.kcfgc
    )

ki18n_wrap_ui(KF5MessageCore
    attachment/ui/attachmentpropertiesdialog.ui
    attachment/ui/attachmentpropertiesdialog_readonly.ui
    )

ecm_qt_declare_logging_category(KF5MessageCore HEADER messagecore_debug.h IDENTIFIER MESSAGECORE_LOG CATEGORY_NAME org.kde.pim.messagecore
        DESCRIPTION "messagelib (messagecore)"
        OLD_CATEGORY_NAMES log_messagecore
        EXPORT MESSAGELIB
    )

ecm_qt_declare_logging_category(KF5MessageCore HEADER autocrypt_debug.h IDENTIFIER AUTOCRYPT_LOG CATEGORY_NAME org.kde.pim.messagecore.autocrypt
        DESCRIPTION "messagelib (messagecore.autocrypt)"
        EXPORT MESSAGELIB
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5MessageCore PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KF5MessageCore BASE_NAME messagecore)

target_link_libraries(KF5MessageCore
    PUBLIC
    KF5::Mime
    PRIVATE
    KF5::KIOCore
    KF5::Codecs
    KF5::PimTextEdit
    KF5::Archive
    KF5::ConfigWidgets
    Qt::Network
    KF5::I18n
    KF5::IdentityManagement
    KF5::MimeTreeParser
    KF5::PimCommon
    )
if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(KF5MessageCore PRIVATE QGpgmeQt6)
else()
    target_link_libraries(KF5MessageCore PRIVATE QGpgme)
endif()

target_include_directories(KF5MessageCore INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/MessageCore/>")

set_target_properties(KF5MessageCore PROPERTIES
    VERSION ${MESSAGECORE_VERSION}
    SOVERSION ${MESSAGECORE_SOVERSION}
    EXPORT_NAME MessageCore
    )

install(TARGETS
    KF5MessageCore
    EXPORT KF5MessageCoreTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS}
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

ecm_generate_headers(MessageCore_Camelcasehelpers_HEADERS
    HEADER_NAMES
    NodeHelper
    REQUIRED_HEADERS MessageCore_helpers_HEADERS
    PREFIX MessageCore
    RELATIVE helpers
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

ecm_generate_pri_file(BASE_NAME MessageCore
    LIB_NAME KF5MessageCore
    DEPS "KMime" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/MessageCore
    )



install(FILES
    ${MessageCore_Camelcaseautocrypt_HEADERS}
    ${MessageCore_Camelcasehelpers_HEADERS}
    ${MessageCore_Camelcasesettings_HEADERS}
    ${MessageCore_Camelcaseutils_HEADERS}
    ${MessageCore_Camelcaseattachment_HEADERS}
    ${MessageCore_Camelcasemisc_HEADERS}
    ${MessageCore_Camelcasemain_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/MessageCore/MessageCore
    COMPONENT Devel
    )

install(FILES
    ${MessageCore_HEADERS}
    ${MessageCore_autocrypt_HEADERS}
    ${MessageCore_helpers_HEADERS}
    ${MessageCore_settings_HEADERS}
    ${MessageCore_utils_HEADERS}
    ${MessageCore_main_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messagecore_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/globalsettings_messagecore.h
    ${MessageCore_attachment_HEADERS}
    ${MessageCore_misc_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/MessageCore/messagecore
    COMPONENT Devel
    )
install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if (BUILD_QCH)
    ecm_add_qch(
        KF5MessageCore_QCH
        NAME KF5MessageCore
        BASE_NAME KF5MessageCore
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${MessageCore_HEADERS}
        ${MessageCore_autocrypt_HEADERS}
        ${MessageCore_helpers_HEADERS}
        ${MessageCore_settings_HEADERS}
        ${MessageCore_utils_HEADERS}
        ${MessageCore_main_HEADERS}
        LINK_QCHS
            Qt5Core_QCH
            Qt5Gui_QCH
            Qt5Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            MESSAGECORE_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )

    ecm_install_qch_export(
        TARGETS KF5MessageCore_QCH
        FILE KF5MessageCoreQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5MessageCoreQchTargets.cmake\")")
endif()

