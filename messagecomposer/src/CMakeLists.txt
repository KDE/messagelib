# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGECOMPOSER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messagecomposer_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageComposerConfigVersion.cmake"
    SOVERSION 6
)

add_definitions(-DTRANSLATION_DOMAIN=\"libmessagecomposer6\")

if(BUILD_TESTING)
    add_subdirectory(imagescaling/autotests)
    add_subdirectory(imagescaling/tests)
    add_subdirectory(composer-ng/autotests)
    add_subdirectory(statusbarwidget/autotests)
    add_subdirectory(plugineditor/autotests)
    add_subdirectory(composer/autotests)
    add_subdirectory(snippet/autotests)
    add_subdirectory(mdn/autotests)
    add_subdirectory(mdn/tests)
    add_subdirectory(contactpreference/autotests)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/part)

add_library( KPim6MessageComposer)
add_library(KPim6::MessageComposer ALIAS KPim6MessageComposer)

set(messagecomposer_interface_SRCS)
qt_add_dbus_interface(messagecomposer_interface_SRCS
    sendlater/org.freedesktop.Akonadi.SendLaterAgent.xml
    sendlaterinterface
)

qt_add_dbus_interface(messagecomposer_interface_SRCS
    followupreminder/org.freedesktop.Akonadi.FollowUpReminder.xml
    followupreminderinterface
)

target_sources(KPim6MessageComposer PRIVATE
    ${messagecomposer_interface_SRCS}
    job/jobbase.cpp
    job/contentjobbase.cpp
    job/attachmentjob.cpp
    job/singlepartjob.cpp
    job/multipartjob.cpp
    job/maintextjob.cpp
    job/signjob.cpp
    job/encryptjob.cpp
    job/signencryptjob.cpp
    job/transparentjob.cpp
    job/inserttextfilejob.cpp
    job/skeletonmessagejob.cpp
    job/aliasesexpandjob.cpp
    job/emailaddressresolvejob.cpp
    job/attachmentfrompublickeyjob.cpp
    job/distributionlistexpandjob.cpp
    job/attachmentvcardfromaddressbookjob.cpp
    job/attachmentclipboardjob.cpp
    job/protectedheadersjob.cpp
    job/autocryptheadersjob.cpp
    job/itipjob.cpp
    statusbarwidget/statusbarlabeltoggledstate.cpp
    composer/autocryptkeyresolvercore.cpp
    composer/composer.cpp
    composer/signaturecontroller.cpp
    composer/composerlineedit.cpp
    composer/composerviewbase.cpp
    composer/keyresolver.cpp
    composer/composerviewinterface.cpp
    composer/composerattachmentinterface.cpp
    contactpreference/contactpreference.cpp
    contactpreference/savecontactpreferencejob.cpp
    contactpreference/contactpreference.h
    contactpreference/savecontactpreferencejob.h
    draftstatus/draftstatus.cpp
    recipient/recipientspicker.cpp
    recipient/recipient.cpp
    recipient/recipientline.cpp
    recipient/recipientseditor.cpp
    recipient/recipientseditorsidewidget.cpp
    recipient/kwindowpositioner.cpp
    recipient/distributionlistdialog.cpp
    snippet/convertsnippetvariablesjob.cpp
    snippet/convertsnippetvariablesutil.cpp
    snippet/convertsnippetvariablemenu.cpp
    imagescaling/imagescaling.cpp
    imagescaling/imagescalingwidget.cpp
    imagescaling/imagescalingutils.cpp
    imagescaling/imagescalingselectformat.cpp
    part/messagepart.cpp
    part/globalpart.cpp
    part/infopart.cpp
    part/itippart.cpp
    part/textpart.cpp
    attachment/attachmentcontrollerbase.cpp
    attachment/attachmentmodel.cpp
    attachment/editorwatcher.cpp
    helper/messagehelper.cpp
    helper/messagefactoryng.cpp
    helper/messagefactoryforwardjob.cpp
    helper/messagefactoryreplyjob.cpp
    sender/akonadisender.cpp
    followupreminder/followupreminderselectdatedialog.cpp
    followupreminder/followupremindercreatejob.cpp
    followupreminder/followupreminder.cpp
    composer-ng/richtextcomposerng.cpp
    composer-ng/richtextcomposersignatures.cpp
    plugineditor/plugineditormanager.cpp
    plugineditor/plugineditor.cpp
    plugineditor/plugineditorinterface.cpp
    plugineditor/pluginactiontype.cpp
    plugineditor/plugincomposerinterface.cpp
    plugineditorcheckbeforesend/plugineditorcheckbeforesend.cpp
    plugineditorcheckbeforesend/plugineditorcheckbeforesendinterface.cpp
    plugineditorcheckbeforesend/plugineditorcheckbeforesendmanager.cpp
    plugineditorcheckbeforesend/plugineditorcheckbeforesendparams.cpp
    plugineditorinit/plugineditorinit.cpp
    plugineditorinit/plugineditorinitmanager.cpp
    plugineditorinit/plugineditorinitinterface.cpp
    plugineditorconverttext/plugineditorconverttext.cpp
    plugineditorconverttext/plugineditorconverttextmanager.cpp
    plugineditorconverttext/plugineditorconverttextinterface.cpp
    plugineditorconverttext/plugineditorconverterinitialdata.cpp
    plugineditorconverttext/plugineditorconverterbeforeconvertingdata.cpp
    plugineditorgrammar/plugineditorgrammarmanager.cpp
    plugineditorgrammar/plugineditorgrammarcustomtoolsviewinterface.cpp
    plugineditorbase/plugineditorbase.cpp
    plugineditorbase/plugineditorconfigurebasewidget.cpp
    sendlater/sendlaterinfo.cpp
    sendlater/sendlaterjob.cpp
    sendlater/sendlatercreatejob.cpp
    sendlater/sendlaterremovejob.cpp
    sendlater/sendlaterutil.cpp
    sendlater/sendlaterdialog.cpp
    sendlater/sendlatertimedatewidget.cpp
    utils/util.cpp
    settings/messagecomposersettings.cpp

    plugineditorinit/plugineditorinit.h
    plugineditorinit/plugineditorinitinterface.h
    plugineditorinit/plugineditorinitmanager.h
    helper/messagefactoryreplyjob.h
    helper/messagefactoryng.h
    helper/messagehelper.h
    helper/messagefactoryforwardjob.h
    job/distributionlistexpandjob.h
    job/contentjobbase.h
    job/attachmentclipboardjob.h
    job/multipartjob.h
    job/aliasesexpandjob.h
    job/signencryptjob.h
    job/inserttextfilejob.h
    job/emailaddressresolvejob.h
    job/singlepartjob.h
    job/attachmentjob.h
    job/jobbase_p.h
    job/transparentjob.h
    job/abstractencryptjob.h
    job/encryptjob.h
    job/protectedheadersjob.h
    job/autocryptheadersjob.h
    job/maintextjob.h
    job/contentjobbase_p.h
    job/signjob.h
    job/skeletonmessagejob.h
    job/attachmentvcardfromaddressbookjob.h
    job/jobbase.h
    job/attachmentfrompublickeyjob.h
    job/itipjob.h
    draftstatus/draftstatus.h
    utils/util_p.h
    utils/kleo_util.h
    utils/util.h
    attachment/attachmentmodel.h
    attachment/attachmentcontrollerbase.h
    attachment/editorwatcher.h
    plugineditorcheckbeforesend/plugineditorcheckbeforesend.h
    plugineditorcheckbeforesend/plugineditorcheckbeforesendparams.h
    plugineditorcheckbeforesend/plugineditorcheckbeforesendinterface.h
    plugineditorcheckbeforesend/plugineditorcheckbeforesendmanager.h
    snippet/convertsnippetvariablesutil.h
    snippet/convertsnippetvariablesjob.h
    snippet/convertsnippetvariablemenu.h
    sendlater/sendlatertimedatewidget_p.h
    sendlater/sendlaterremovejob.h
    sendlater/sendlatercreatejob.h
    sendlater/sendlaterutil.h
    sendlater/sendlaterdialog.h
    sendlater/sendlaterjob.h
    sendlater/sendlaterutil_p.h
    sendlater/sendlaterinfo.h
    statusbarwidget/statusbarlabeltoggledstate.h
    followupreminder/followupreminderselectdatedialog.h
    followupreminder/followupreminder.h
    followupreminder/followupremindercreatejob.h
    messagecomposer_private_export.h
    recipient/distributionlistdialog.h
    recipient/recipientseditor.h
    recipient/recipient.h
    recipient/kwindowpositioner.h
    recipient/recipientspicker.h
    recipient/recipientseditorsidewidget.h
    recipient/recipientline.h
    composer/autocryptkeyresolvercore.h
    composer/composerviewinterface.h
    composer/composer.h
    composer/keyresolver.h
    composer/composerattachmentinterface.h
    composer/signaturecontroller.h
    composer/composerviewbase.h
    composer/composerlineedit.h
    plugineditorconverttext/plugineditorconverttextmanager.h
    plugineditorconverttext/plugineditorconverterbeforeconvertingdata.h
    plugineditorconverttext/plugineditorconverttextinterface.h
    plugineditorconverttext/plugineditorconverttext.h
    plugineditorconverttext/plugineditorconverterinitialdata.h
    plugineditorgrammar/plugineditorgrammarcustomtoolsviewinterface.h
    plugineditorgrammar/plugineditorgrammarmanager.h
    plugineditorbase/plugineditorbase.h
    plugineditorbase/plugineditorconfigurebasewidget.h
    imagescaling/imagescalingselectformat.h
    imagescaling/imagescalingwidget.h
    imagescaling/imagescaling.h
    imagescaling/imagescalingutils.h
    composer-ng/richtextcomposersignatures.h
    composer-ng/richtextcomposerng.h
    plugineditor/plugineditorinterface.h
    plugineditor/pluginactiontype.h
    plugineditor/plugincomposerinterface.h
    plugineditor/plugineditor.h
    plugineditor/plugineditormanager.h
    settings/messagecomposersettings.h
    part/infopart.h
    part/itippart.h
    part/textpart.h
    part/messagepart.h
    part/globalpart.h
    sender/akonadisender.h
    sender/messagesender.h

    mdn/mdnadvicehelper.cpp
    mdn/mdnadvicehelper.h
    mdn/mdnadvicedialog.cpp
    mdn/mdnadvicedialog.h
    mdn/mdnwarningwidgetjob.cpp
    mdn/mdnwarningwidgetjob.h
)


ki18n_wrap_ui(KPim6MessageComposer
    imagescaling/ui/imagescalingwidget.ui
    sendlater/ui/sendlaterwidget.ui
)
ecm_qt_declare_logging_category(KPim6MessageComposer HEADER messagecomposer_debug.h IDENTIFIER MESSAGECOMPOSER_LOG CATEGORY_NAME org.kde.pim.messagecomposer
    DESCRIPTION "messagelib (messagecomposer)"
    OLD_CATEGORY_NAMES log_messagecomposer
    EXPORT MESSAGELIB
)


if(KDEPIM_ENTERPRISE_BUILD)
    set(WARN_TOOMANY_RECIPIENTS_DEFAULT true)
    set(ALLOW_SEMICOLON_AS_ADDRESS_SEPARATOR_DEFAULT true)
else()
    set(WARN_TOOMANY_RECIPIENTS_DEFAULT false)
    set(ALLOW_SEMICOLON_AS_ADDRESS_SEPARATOR_DEFAULT false)
endif()

configure_file(settings/messagecomposer.kcfg.in ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer.kcfg)

kconfig_add_kcfg_files(KPim6MessageComposer
    settings/messagecomposersettings_base.kcfgc
)

if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim6MessageComposer PROPERTIES UNITY_BUILD ON)
endif()
ecm_generate_export_header(KPim6MessageComposer
    BASE_NAME messagecomposer
    VERSION ${MESSAGECOMPOSER_VERSION}
    DEPRECATED_BASE_VERSION 0
    USE_VERSION_HEADER
)

target_link_libraries(KPim6MessageComposer
    PUBLIC
    KPim6::Mime
    KPim6::MessageCore
    KPim6::PimCommonAkonadi
    KPim6::AkonadiCore
    KPim6::IdentityManagementWidgets
    KPim6::IdentityManagementCore
    KPim6::PimTextEdit
    KPim6::AkonadiMime
    KPim6::Libkleo
    PRIVATE
    KPim6::MailTransport
    KPim6::PimTextEdit
    KPim6::TemplateParser
    KPim6::MessageViewer
    KPim6::AkonadiWidgets
    KF6::KIOCore
    KF6::I18n
    KF6::KIOFileWidgets # for KEncodingDialog
    KF6::XmlGui         # for KActionCollection
    KF6::SonnetUi
    KF6::CalendarCore   # for KCalendarCore/Todo
    KF6::Archive
    KF6::Contacts
    KF6::SonnetCore
    KF6::GuiAddons
    KF6::Codecs
    ${Inotify_LIBRARIES}
)
if(TARGET KF6::TextAutoCorrectionWidgets)
    target_link_libraries(KPim6MessageComposer
        PUBLIC
        KF6::TextAutoCorrectionWidgets
)
else()
    target_link_libraries(KPim6MessageComposer
        PUBLIC
        KF6::TextAutoCorrection
)
endif()

target_include_directories(KPim6MessageComposer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageComposer/>")

set_target_properties(KPim6MessageComposer PROPERTIES
    VERSION ${MESSAGECOMPOSER_VERSION}
    SOVERSION ${MESSAGECOMPOSER_SOVERSION}
    EXPORT_NAME MessageComposer
)

install(TARGETS
    KPim6MessageComposer
    EXPORT KPim6MessageComposerTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)

ecm_generate_headers(MessageComposer_Camelstatusbarwidget_HEADERS
    HEADER_NAMES
    StatusBarLabelToggledState
    REQUIRED_HEADERS MessageComposer_statusbarwidget_HEADERS
    PREFIX MessageComposer
    RELATIVE statusbarwidget
)

ecm_generate_headers(MessageComposer_Camelsnippet_HEADERS
    HEADER_NAMES
    ConvertSnippetVariablesJob
    ConvertSnippetVariablesUtil
    ConvertSnippetVariableMenu
    REQUIRED_HEADERS MessageComposer_snippet_HEADERS
    PREFIX MessageComposer
    RELATIVE snippet
)

ecm_generate_headers(MessageComposer_Camelplugineditor_HEADERS
    HEADER_NAMES
    PluginEditor
    PluginActionType
    PluginEditorInterface
    PluginComposerInterface
    PluginEditorManager
    REQUIRED_HEADERS MessageComposer_plugineditor_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditor
)

ecm_generate_headers(MessageComposer_Camelplugineditorinit_HEADERS
    HEADER_NAMES
    PluginEditorInit
    PluginEditorInitManager
    PluginEditorInitInterface
    REQUIRED_HEADERS MessageComposer_plugineditorinit_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorinit
)

ecm_generate_headers(MessageComposer_CamelPluginEditorBase_HEADERS
    HEADER_NAMES
    PluginEditorBase
    PluginEditorConfigureBaseWidget
    REQUIRED_HEADERS MessageComposer_PluginEditorBase_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorbase
)

ecm_generate_headers(MessageComposer_Camelplugineditorconverttext_HEADERS
    HEADER_NAMES
    PluginEditorConvertText
    PluginEditorConvertTextManager
    PluginEditorConvertTextInterface
    PluginEditorConverterInitialData
    PluginEditorConverterBeforeConvertingData
    REQUIRED_HEADERS MessageComposer_plugineditorconverttext_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorconverttext
)

ecm_generate_headers(MessageComposer_Camelplugineditorgrammar_HEADERS
    HEADER_NAMES
    PluginEditorGrammarManager
    PluginEditorGrammarCustomToolsViewInterface
    REQUIRED_HEADERS MessageComposer_plugineditorgrammar_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorgrammar
)


ecm_generate_headers(MessageComposer_Camelplugineditorcheckbeforesend_HEADERS
    HEADER_NAMES
    PluginEditorCheckBeforeSend
    PluginEditorCheckBeforeSendInterface
    PluginEditorCheckBeforeSendManager
    PluginEditorCheckBeforeSendParams
    REQUIRED_HEADERS MessageComposer_plugineditorcheckbeforesend_HEADERS
    PREFIX MessageComposer
    RELATIVE plugineditorcheckbeforesend
)


ecm_generate_headers(MessageComposer_Camelcaseattachement_HEADERS
    HEADER_NAMES
    AttachmentModel
    AttachmentControllerBase
    EditorWatcher
    REQUIRED_HEADERS MessageComposer_attachement_HEADERS
    PREFIX MessageComposer
    RELATIVE attachment
)

ecm_generate_headers(MessageComposer_Camelcasecomposer_HEADERS
    HEADER_NAMES
    AutocryptKeyResolverCore
    Composer
    ComposerLineEdit
    ComposerViewBase
    ComposerViewInterface
    ComposerAttachmentInterface
    KeyResolver
    SignatureController
    REQUIRED_HEADERS MessageComposer_composer_HEADERS
    PREFIX MessageComposer
    RELATIVE composer
)

ecm_generate_headers(MessageComposer_Camelcasecomposerng_HEADERS
    HEADER_NAMES
    RichTextComposerNg
    RichTextComposerSignatures
    REQUIRED_HEADERS MessageComposer_composerng_HEADERS
    PREFIX MessageComposer
    RELATIVE composer-ng
)

ecm_generate_headers(MessageComposer_Camelcasecontactpreference_HEADERS
    HEADER_NAMES
    SaveContactPreferenceJob
    ContactPreference
    REQUIRED_HEADERS MessageComposer_contactpreference_HEADERS
    PREFIX MessageComposer
    RELATIVE contactpreference
)

ecm_generate_headers(MessageComposer_Camelcasesender_HEADERS
    HEADER_NAMES
    AkonadiSender
    MessageSender
    REQUIRED_HEADERS MessageComposer_sender_HEADERS
    PREFIX MessageComposer
    RELATIVE sender
)

ecm_generate_headers(MessageComposer_Camelcaseutils_HEADERS
    HEADER_NAMES
    Util
    Kleo_Util
    REQUIRED_HEADERS MessageComposer_utils_HEADERS
    PREFIX MessageComposer
    RELATIVE utils
)

ecm_generate_headers(MessageComposer_Camelcasehelper_HEADERS
    HEADER_NAMES
    MessageHelper
    MessageFactoryNG
    REQUIRED_HEADERS MessageComposer_helper_HEADERS
    PREFIX MessageComposer
    RELATIVE helper
)

ecm_generate_headers(MessageComposer_Camelcasesettings_HEADERS
    HEADER_NAMES
    MessageComposerSettings
    REQUIRED_HEADERS MessageComposer_settings_HEADERS
    PREFIX MessageComposer
    RELATIVE settings
)

ecm_generate_headers(MessageComposer_Camelcasepart_HEADERS
    HEADER_NAMES
    TextPart
    GlobalPart
    InfoPart
    ItipPart
    MessagePart
    REQUIRED_HEADERS MessageComposer_part_HEADERS
    PREFIX MessageComposer
    RELATIVE part
)

ecm_generate_headers(MessageComposer_Camelcasefollowupreminder_HEADERS
    HEADER_NAMES
    FollowupReminderCreateJob
    FollowUpReminderSelectDateDialog
    FollowupReminder
    REQUIRED_HEADERS MessageComposer_followupreminder_HEADERS
    PREFIX MessageComposer
    RELATIVE followupreminder
)

ecm_generate_headers(MessageComposer_Camelcaserecipient_HEADERS
    HEADER_NAMES
    Recipient
    RecipientsEditor
    RecipientLine
    REQUIRED_HEADERS MessageComposer_recipient_HEADERS
    PREFIX MessageComposer
    RELATIVE recipient
)

ecm_generate_headers(MessageComposer_Camelcaseimagescaling_HEADERS
    HEADER_NAMES
    ImageScalingWidget
    REQUIRED_HEADERS MessageComposer_imagescaling_HEADERS
    PREFIX MessageComposer
    RELATIVE imagescaling
)

ecm_generate_headers(MessageComposer_Camelcasesendlater_HEADERS
    HEADER_NAMES
    SendLaterInfo
    SendLaterDialog
    SendLaterUtil
    SendLaterJob
    SendLaterCreateJob
    SendLaterRemoveJob
    REQUIRED_HEADERS MessageComposer_sendlater_HEADERS
    PREFIX MessageComposer
    RELATIVE sendlater
)

ecm_generate_headers(MessageComposer_Camelcasejob_HEADERS
    HEADER_NAMES
    JobBase
    AbstractEncryptJob
    ContentJobBase
    InsertTextFileJob
    AttachmentJob
    SinglepartJob
    MainTextJob
    AttachmentFromPublicKeyJob
    MultipartJob
    EncryptJob
    AutocryptHeadersJob
    ProtectedHeadersJob
    AttachmentVcardFromAddressBookJob
    SignJob
    SignEncryptJob
    TransparentJob
    JobBase
    AliasesExpandJob
    SkeletonMessageJob
    AttachmentClipBoardJob
    ItipJob
    REQUIRED_HEADERS MessageComposer_job_HEADERS
    PREFIX MessageComposer
    RELATIVE job
)

ecm_generate_headers(MessageComposer_Camelcasedraftstatus_HEADERS
    HEADER_NAMES
    DraftStatus
    REQUIRED_HEADERS MessageComposer_draftstatus_HEADERS
    PREFIX MessageComposer
    RELATIVE draftstatus
)

ecm_generate_headers(MessageComposer_CamelcaseMdn_HEADERS
    HEADER_NAMES
    MDNAdviceHelper
    MDNAdviceDialog
    MDNWarningWidgetJob
    REQUIRED_HEADERS MessageComposer_mdn_HEADERS
    PREFIX MessageComposer
    RELATIVE mdn
)

install(FILES
    ${MessageComposer_CamelcaseMdn_HEADERS}
    ${MessageComposer_CamelPluginEditorBase_HEADERS}
    ${MessageComposer_Camelsnippet_HEADERS}
    ${MessageComposer_Camelplugineditorconverttext_HEADERS}
    ${MessageComposer_Camelplugineditorinit_HEADERS}
    ${MessageComposer_Camelplugineditorcheckbeforesend_HEADERS}
    ${MessageComposer_Camelcasecomposer_HEADERS}
    ${MessageComposer_Camelcasecomposerng_HEADERS}
    ${MessageComposer_Camelcasecontactpreference_HEADERS}
    ${MessageComposer_Camelcasesender_HEADERS}
    ${MessageComposer_Camelcaseutils_HEADERS}
    ${MessageComposer_Camelcasehelper_HEADERS}
    ${MessageComposer_Camelcasesettings_HEADERS}
    ${MessageComposer_Camelcasepart_HEADERS}
    ${MessageComposer_Camelcasefollowupreminder_HEADERS}
    ${MessageComposer_Camelcaserecipient_HEADERS}
    ${MessageComposer_Camelcaseimagescaling_HEADERS}
    ${MessageComposer_Camelcasesendlater_HEADERS}
    ${MessageComposer_Camelcasejob_HEADERS}
    ${MessageComposer_Camelcaseattachement_HEADERS}
    ${MessageComposer_Camelplugineditor_HEADERS}
    ${MessageComposer_Camelplugineditorgrammar_HEADERS}
    ${MessageComposer_Camelstatusbarwidget_HEADERS}
    ${MessageComposer_Camelcasedraftstatus_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageComposer/MessageComposer
    COMPONENT Devel
)

install(FILES
    ${MessageComposer_mdn_HEADERS}
    ${MessageComposer_HEADERS}
    ${MessageComposer_PluginEditorBase_HEADERS}
    ${MessageComposer_snippet_HEADERS}
    ${MessageComposer_plugineditorconverttext_HEADERS}
    ${MessageComposer_plugineditorinit_HEADERS}
    ${MessageComposer_statusbarwidget_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/messagecomposersettings_base.h
    ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer_debug.h
    ${MessageComposer_composer_HEADERS}
    ${MessageComposer_composerng_HEADERS}
    ${MessageComposer_contactpreference_HEADERS}
    ${MessageComposer_sender_HEADERS}
    ${MessageComposer_utils_HEADERS}
    ${MessageComposer_helper_HEADERS}
    ${MessageComposer_settings_HEADERS}
    ${MessageComposer_part_HEADERS}
    ${MessageComposer_followupreminder_HEADERS}
    ${MessageComposer_recipient_HEADERS}
    ${MessageComposer_imagescaling_HEADERS}
    ${MessageComposer_sendlater_HEADERS}
    ${MessageComposer_attachement_HEADERS}
    ${MessageComposer_job_HEADERS}
    ${MessageComposer_plugineditor_HEADERS}
    ${MessageComposer_plugineditorcheckbeforesend_HEADERS}
    ${MessageComposer_plugineditorgrammar_HEADERS}
    ${MessageComposer_draftstatus_HEADERS}

    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageComposer/messagecomposer
    COMPONENT Devel
)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MessageComposer")

if(BUILD_QCH)
    ecm_add_qch(
        KPim6MessageComposer_QCH
        NAME KPim6MessageComposer
        BASE_NAME KPim6MessageComposer
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${MessageComposer_HEADERS}
        ${MessageComposer_PluginEditorBase_HEADERS}
        ${MessageComposer_snippet_HEADERS}
        ${MessageComposer_plugineditorconverttext_HEADERS}
        ${MessageComposer_plugineditorinit_HEADERS}
        ${MessageComposer_statusbarwidget_HEADERS}
        ${MessageComposer_composer_HEADERS}
        ${MessageComposer_composerng_HEADERS}
        ${MessageComposer_sender_HEADERS}
        ${MessageComposer_utils_HEADERS}
        ${MessageComposer_helper_HEADERS}
        ${MessageComposer_settings_HEADERS}
        ${MessageComposer_part_HEADERS}
        ${MessageComposer_followupreminder_HEADERS}
        ${MessageComposer_recipient_HEADERS}
        ${MessageComposer_imagescaling_HEADERS}
        ${MessageComposer_sendlater_HEADERS}
        ${MessageComposer_attachement_HEADERS}
        ${MessageComposer_job_HEADERS}
        ${MessageComposer_plugineditor_HEADERS}
        ${MessageComposer_plugineditorcheckbeforesend_HEADERS}
        ${MessageComposer_plugineditorgrammar_HEADERS}
        ${MessageComposer_draftstatus_HEADERS}
        LINK_QCHS
        Qt6Core_QCH
        Qt6Gui_QCH
        Qt6Widgets_QCH
        INCLUDE_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
        MESSAGECOMPOSER_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
)

ecm_install_qch_export(
    TARGETS KPim6MessageComposer_QCH
    FILE KPim6MessageComposerQchTargets.cmake
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)
set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim6MessageComposerQchTargets.cmake\")")

endif()

########### CMake Config Files ###########

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMessageComposerConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageComposerConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageComposerConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageComposerConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT KPim6MessageComposerTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MessageComposerTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageComposer COMPONENT Devel
)
