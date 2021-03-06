
add_definitions(-DTRANSLATION_DOMAIN=\"libmessagecomposer\")



if(BUILD_TESTING)
    add_subdirectory( imagescaling/autotests )
    add_subdirectory( imagescaling/tests )
    add_subdirectory( composer-ng/autotests )
    add_subdirectory( statusbarwidget/autotests )
    add_subdirectory( plugineditor/autotests )
    add_subdirectory( composer/autotests )
    add_subdirectory( snippet/autotests )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/part)

set( messagecomposer_job_src
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
    job/savecontactpreferencejob.cpp
    job/attachmentvcardfromaddressbookjob.cpp
    job/attachmentclipboardjob.cpp
    job/protectedheadersjob.cpp
    job/autocryptheadersjob.cpp
    )

set( messagecomposer_statusbarwidget_src
    statusbarwidget/statusbarlabeltoggledstate.cpp
   )

set( messagecomposer_composer_src
    composer/composer.cpp
    composer/signaturecontroller.cpp
    composer/composerlineedit.cpp
    composer/composerviewbase.cpp
    composer/keyresolver.cpp
    composer/composerviewinterface.cpp
    composer/composerattachmentinterface.cpp
    )

set( messagecomposer_recipient_src
    recipient/recipientspicker.cpp
    recipient/recipient.cpp
    recipient/recipientline.cpp
    recipient/recipientseditor.cpp
    recipient/recipientseditorsidewidget.cpp
    recipient/kwindowpositioner.cpp
    recipient/distributionlistdialog.cpp
    )

set( messagecomposer_snippet_src
    snippet/convertsnippetvariablesjob.cpp
    snippet/convertsnippetvariablesutil.cpp
    snippet/convertsnippetvariablemenu.cpp
    )

set( messagecomposer_imagescaling_src
    imagescaling/imagescaling.cpp
    imagescaling/imagescalingwidget.cpp
    imagescaling/imagescalingutils.cpp
    imagescaling/imagescalingselectformat.cpp
    )

set( messagecomposer_part_src
    part/messagepart.cpp
    part/globalpart.cpp
    part/infopart.cpp
    part/textpart.cpp
    )

set( messagecomposer_attachment_src
    attachment/attachmentcontrollerbase.cpp
    attachment/attachmentmodel.cpp
    )

set( messagecomposer_helper_src
    helper/messagehelper.cpp
    helper/messagefactoryng.cpp
    helper/messagefactoryforwardjob.cpp
    helper/messagefactoryreplyjob.cpp
    )

set( messagecomposer_sender_src
    sender/akonadisender.cpp
    )

set(messagecomposer_followupreminder_SRCS
    followupreminder/followupreminderselectdatedialog.cpp
    followupreminder/followupremindercreatejob.cpp
    followupreminder/followupreminder.cpp
    )


set(messagecomposer_richtextcomposerng_SRCS
    composer-ng/richtextcomposerng.cpp
    composer-ng/richtextcomposersignatures.cpp
    )

set(messagecomposer_plugineditor_SRCS
    plugineditor/plugineditormanager.cpp
    plugineditor/plugineditor.cpp
    plugineditor/plugineditorinterface.cpp
    plugineditor/pluginactiontype.cpp
    plugineditor/plugincomposerinterface.cpp
    )

set(messagecomposer_plugineditorcheckbeforesend_SRCS
   plugineditorcheckbeforesend/plugineditorcheckbeforesend.cpp
   plugineditorcheckbeforesend/plugineditorcheckbeforesendinterface.cpp
   plugineditorcheckbeforesend/plugineditorcheckbeforesendmanager.cpp
   plugineditorcheckbeforesend/plugineditorcheckbeforesendparams.cpp
   )

set(messagecomposer_plugineditorinit_SRCS
   plugineditorinit/plugineditorinit.cpp
   plugineditorinit/plugineditorinitmanager.cpp
   plugineditorinit/plugineditorinitinterface.cpp
   )

set(messagecomposer_plugineditorconverttext_SRCS
   plugineditorconverttext/plugineditorconverttext.cpp
   plugineditorconverttext/plugineditorconverttextmanager.cpp
   plugineditorconverttext/plugineditorconverttextinterface.cpp
   plugineditorconverttext/plugineditorconverterinitialdata.cpp
   plugineditorconverttext/plugineditorconverterbeforeconvertingdata.cpp
   )
set(messagecomposer_plugineditorgrammar_SRCS
    plugineditorgrammar/plugineditorgrammarmanager.cpp
    plugineditorgrammar/plugineditorgrammarcustomtoolsviewinterface.cpp
    )

set(messagecomposer_plugineditorbase_SRCS
   plugineditorbase/plugineditorbase.cpp
   plugineditorbase/plugineditorconfigurebasewidget.cpp
   )
set(messagecomposer_sendlater_SRCS
    sendlater/sendlaterinfo.cpp
    sendlater/sendlaterjob.cpp
    sendlater/sendlatercreatejob.cpp
    sendlater/sendlaterremovejob.cpp
    sendlater/sendlaterutil.cpp
    sendlater/sendlaterdialog.cpp
    sendlater/sendlatertimedatewidget.cpp
)

qt_add_dbus_interface(messagecomposer_sendlater_SRCS
    sendlater/org.freedesktop.Akonadi.SendLaterAgent.xml
    sendlaterinterface
)

qt_add_dbus_interface(messagecomposer_followupreminder_SRCS
    followupreminder/org.freedesktop.Akonadi.FollowUpReminder.xml
    followupreminderinterface
    )


set( messagecomposer_src
    ${messagecomposer_snippet_src}
    ${messagecomposer_statusbarwidget_src}
    ${messagecomposer_plugineditorconverttext_SRCS}
    ${messagecomposer_plugineditorinit_SRCS}
    ${messagecomposer_plugineditor_SRCS}
    ${messagecomposer_richtextcomposerng_SRCS}
    ${messagecomposer_part_src}
    ${messagecomposer_imagescaling_src}
    ${messagecomposer_job_src}
    ${messagecomposer_composer_src}
    ${messagecomposer_recipient_src}
    ${messagecomposer_attachment_src}
    ${messagecomposer_helper_src}
    ${messagecomposer_sender_src}
    ${messagecomposer_followupreminder_SRCS}
    ${messagecomposer_plugineditorcheckbeforesend_SRCS}
    ${messagecomposer_plugineditorgrammar_SRCS}
    ${messagecomposer_sendlater_SRCS}
    ${messagecomposer_plugineditorbase_SRCS}
    utils/util.cpp
    settings/messagecomposersettings.cpp
    )

ki18n_wrap_ui(messagecomposer_src
    imagescaling/ui/imagescalingwidget.ui
    sendlater/ui/sendlaterwidget.ui
    )
ecm_qt_declare_logging_category(messagecomposer_src HEADER messagecomposer_debug.h IDENTIFIER MESSAGECOMPOSER_LOG CATEGORY_NAME org.kde.pim.messagecomposer
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

configure_file(settings/messagecomposer.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/messagecomposer.kcfg)

kconfig_add_kcfg_files(messagecomposer_src
    settings/messagecomposersettings_base.kcfgc
    )

add_library( KF5MessageComposer ${messagecomposer_src} )
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5MessageComposer PROPERTIES UNITY_BUILD ON)
endif()
generate_export_header(KF5MessageComposer BASE_NAME messagecomposer)
add_library(KF5::MessageComposer ALIAS KF5MessageComposer)
target_link_libraries(KF5MessageComposer
    PUBLIC
    KF5::Mime
    KF5::MessageCore
    KF5::PimCommonAkonadi
    KF5::AkonadiCore
    KF5::IdentityManagement
    KF5::AkonadiMime
    KF5::Libkleo
    KF5::MessageViewer
    PRIVATE
    KF5::MailTransportAkonadi
    KF5::PimTextEdit
    KF5::TemplateParser
    KF5::AkonadiWidgets
    KF5::PimCommonAkonadi
    KF5::KIOCore
    KF5::I18n
    KF5::KIOWidgets     # for KIO::JobUiDelegate
    KF5::KIOFileWidgets # for KEncodingDialog
    KF5::XmlGui         # for KActionCollection
    KF5::SonnetUi
    KF5::CalendarCore   # for KCalendarCore/Todo
    KF5::Archive
    KF5::Contacts
    KF5::SonnetCore
    KF5::GuiAddons
    )

target_include_directories(KF5MessageComposer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/MessageComposer/;${KDE_INSTALL_INCLUDEDIR_KF5}/messagecomposer>")

set_target_properties(KF5MessageComposer PROPERTIES
    VERSION ${MESSAGECOMPOSER_VERSION}
    SOVERSION ${MESSAGECOMPOSER_SOVERSION}
    EXPORT_NAME MessageComposer
    )

install(TARGETS
    KF5MessageComposer
    EXPORT KF5MessageComposerTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
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
    REQUIRED_HEADERS MessageComposer_attachement_HEADERS
    PREFIX MessageComposer
    RELATIVE attachment
    )

ecm_generate_headers(MessageComposer_Camelcasecomposer_HEADERS
    HEADER_NAMES
    Composer
    ComposerLineEdit
    ComposerViewBase
    ComposerViewInterface
    ComposerAttachmentInterface
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
    REQUIRED_HEADERS MessageComposer_job_HEADERS
    PREFIX MessageComposer
    RELATIVE job
    )
ecm_generate_pri_file(BASE_NAME MessageComposer
    LIB_NAME KF5MessageComposer
    DEPS "KMime MessageCore PimCommon Akonadi KIdentityManagement AkonadiMime Libkleo" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageComposer
    )

install(FILES
    ${MessageComposer_CamelPluginEditorBase_HEADERS}
    ${MessageComposer_Camelsnippet_HEADERS}
    ${MessageComposer_Camelplugineditorconverttext_HEADERS}
    ${MessageComposer_Camelplugineditorinit_HEADERS}
    ${MessageComposer_Camelplugineditorcheckbeforesend_HEADERS}
    ${MessageComposer_Camelcasecomposer_HEADERS}
    ${MessageComposer_Camelcasecomposerng_HEADERS}
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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageComposer
    COMPONENT Devel
    )

install(FILES
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

    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/messagecomposer
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

