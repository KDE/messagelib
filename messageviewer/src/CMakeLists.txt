# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

add_definitions(-DTRANSLATION_DOMAIN=\"libmessageviewer\")

add_subdirectory(messagepartthemes/grantlee)

# KCFG files:
# The main messageviewer.kcfg is configured by CMake and put in the build directory.

if(KDEPIM_ENTERPRISE_BUILD)
    set(LEGACY_MANGLE_FROM_TO_HEADERS true)
    set(LEGACY_BODY_INVITES true)
    set(EXCHANGE_COMPATIBLE_INVITATIONS true)
else()
    set(LEGACY_MANGLE_FROM_TO_HEADERS false)
    set(LEGACY_BODY_INVITES false)
    set(EXCHANGE_COMPATIBLE_INVITATIONS false)
endif()

configure_file(settings/messageviewer.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/messageviewer.kcfg)
include(CheckIncludeFiles)
find_package(Inotify)
set_package_properties(Inotify PROPERTIES
                       PURPOSE "Filesystem alteration notifications using inotify")
if(Inotify_FOUND)
    set(HAVE_SYS_INOTIFY_H 1)
else()
    set(HAVE_SYS_INOTIFY_H 0)
endif()

find_package(Qca-qt${QT_MAJOR_VERSION} 2.2.0 CONFIG REQUIRED)
set_package_properties(Qca-qt${QT_MAJOR_VERSION} PROPERTIES DESCRIPTION "Qt Cryptographic Architecture"
                   URL "https://invent.kde.org/libraries/qca" TYPE REQUIRED
                   PURPOSE "Needed for dkim support.")


configure_file(config-messageviewer.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-messageviewer.h)

# target_include_directories does not handle empty include paths
include_directories(${GPGME_INCLUDES})

add_library(KF5MessageViewer)
add_library(KF5::MessageViewer ALIAS KF5MessageViewer)


if(BUILD_TESTING)
    add_subdirectory(header/autotests)
    add_subdirectory(scamdetection/autotests)
    add_subdirectory(scamdetection/tests)
    add_subdirectory(viewerplugins/tests/)
    add_subdirectory(htmlwriter/autotests)
    add_subdirectory(viewer/webengine/tests)
    add_subdirectory(messagepartthemes/default/autotests)
    add_subdirectory(widgets/autotests/)
    add_subdirectory(utils/autotests)
    add_subdirectory(viewer/renderer/autotests)
    add_subdirectory(remote-content/autotests)
    add_subdirectory(openurlwith/autotests)
    add_subdirectory(mdn/autotests)
endif()
add_subdirectory(pics)
add_subdirectory(about)
add_subdirectory(messageviewerheaderplugins)

if (BUILD_TESTING)
    add_subdirectory(dkim-verify/autotests)
    add_subdirectory(dkim-verify/tests)
endif()
target_sources(KF5MessageViewer PRIVATE
    dkim-verify/dkiminfo.cpp
    dkim-verify/dkimmanagerkey.cpp
    dkim-verify/dkimmanagerkeywidget.cpp
    dkim-verify/dkimmanagerkeydialog.cpp
    dkim-verify/dkimdownloadkeyjob.cpp
    dkim-verify/dkimchecksignaturejob.cpp
    dkim-verify/dkimcheckauthenticationstatusjob.cpp
    dkim-verify/dkimauthenticationstatusinfo.cpp
    dkim-verify/dkimutil.cpp
    dkim-verify/dkimkeyrecord.cpp
    dkim-verify/dkimmanager.cpp
    dkim-verify/dkimresultattribute.cpp
    dkim-verify/dkimwidgetinfo.cpp
    dkim-verify/dkimstoreresultjob.cpp
    dkim-verify/dkimheaderparser.cpp
    dkim-verify/dkimcheckpolicy.cpp
    dkim-verify/dmarcrecordjob.cpp
    dkim-verify/dmarcinfo.cpp
    dkim-verify/dmarcpolicyjob.cpp
    dkim-verify/dkimcheckpolicyjob.cpp
    dkim-verify/dkimrule.cpp
    dkim-verify/dkimmanagerrules.cpp
    dkim-verify/dkimgeneraterulejob.cpp
    dkim-verify/dkimauthenticationstatusinfoutil.cpp
    dkim-verify/dkimcheckfulljob.cpp
    dkim-verify/dkimauthenticationstatusinfoconverter.cpp
    dkim-verify/dkimmanagerauthenticationserver.cpp
    dkim-verify/dkimviewermenu.cpp
    dkim-verify/dkimmanagerulescombobox.cpp
    dkim-verify/dkimmanagerulesdialog.cpp
    dkim-verify/dkimmanageruleswidget.cpp
    dkim-verify/dkimruledialog.cpp
    dkim-verify/dkimrulewidget.cpp
    dkim-verify/dmarcmanager.cpp
)


if(DEBUG_SIGNATURE)
    add_definitions(-DDEBUG_SIGNATURE)
endif()
if (MESSAGEVIEWER_EXPERIMENTAL_CONVERSATIONVIEW)
    target_sources(KF5MessageViewer PRIVATE
        conversationview/conversationviewwidget.cpp
        conversationview/conversationviewwidget.h
        )
    if (BUILD_TESTING)
        add_subdirectory(conversationview/autotests)
        add_subdirectory(conversationview/tests)
    endif()
endif()
target_sources(KF5MessageViewer PRIVATE
    viewer/renderer/messageviewerrenderer.cpp
    viewer/webengine/mailwebengineview.cpp
    viewer/webengine/mailwebenginepage.cpp
    viewer/webengine/loadexternalreferencesurlinterceptor/loadexternalreferencesurlinterceptor.cpp
    viewer/webengine/cidreferencesurlinterceptor/cidreferencesurlinterceptor.cpp
    viewer/webengine/cidschemehandler/cidschemehandler.cpp
    viewer/attachmentstrategy.cpp
    viewer/csshelper.cpp
    viewer/csshelperbase.cpp
    viewer/editorwatcher.cpp
    viewer/objecttreeemptysource.cpp
    viewer/objecttreeviewersource.cpp
    viewer/viewer.cpp
    viewer/viewer_p.cpp
    viewer/messagedisplayformatattribute.cpp
    viewer/urlhandlermanager.cpp
    viewer/mimeparttree/mimeparttreeview.cpp
    viewer/mimeparttree/mimetreemodel.cpp
    viewer/viewerpurposemenuwidget.cpp
    viewer/messagewidgetarea.cpp
    widgets/attachmentdialog.cpp
    widgets/configurewidget.cpp
    widgets/printingsettings.cpp
    widgets/htmlstatusbar.cpp
    widgets/vcardviewer.cpp
    widgets/invitationsettings.cpp
    widgets/opensavedfilefolderwidget.cpp
    widgets/mailsourceviewtextbrowserwidget.cpp
    widgets/shownextmessagewidget.cpp
    widgets/mailsourcewebengineviewer.cpp
    header/contactdisplaymessagememento.cpp
    header/headerstrategy.cpp
    header/richheaderstrategy.cpp
    header/headerstyle.cpp
    header/grantleeheaderstyle.cpp
    header/plainheaderstyle.cpp
    header/headerstyle_util.cpp
    header/grantleeheaderformatter.cpp
    header/grantleeheaderteststyle.cpp
    header/kxface.cpp
    header/headerstyleplugin.cpp
    header/headerstylepluginmanager.cpp
    header/headerstyleinterface.cpp
    header/headerstylemenumanager.cpp
    scamdetection/scamdetectionwarningwidget.cpp
    scamdetection/scamdetectiondetailsdialog.cpp
    scamdetection/scamattribute.cpp
    scamdetection/scamcheckshorturl.cpp
    scamdetection/scamexpandurljob.cpp
    scamdetection/scamcheckshorturlmanager.cpp
    scamdetection/scamdetectionwebengine.cpp
    scamdetection/scamdetectionwhitelistsettingsmanager.cpp
    scamdetection/scamdetectioninfo.cpp
    findbar/findbarsourceview.cpp
    utils/iconnamecache.cpp
    utils/markmessagereadhandler.cpp
    utils/messageviewerutil.cpp
    utils/mimetype.cpp
    htmlwriter/webengineparthtmlwriter.cpp
    htmlwriter/webengineembedpart.cpp
    htmlwriter/bufferedhtmlwriter.cpp
    htmlwriter/filehtmlwriter.cpp
    antispam/spamheaderanalyzer.cpp
    antispam/antispamconfig.cpp
    job/modifymessagedisplayformatjob.cpp
    viewerplugins/viewerpluginmanager.cpp
    viewerplugins/viewerplugin.cpp
    viewerplugins/viewerplugininterface.cpp
    viewerplugins/viewerplugintoolmanager.cpp
    messageviewerconfigureplugins/messageviewerconfiguresettingsplugin.cpp
    messageviewerconfigureplugins/messageviewerconfiguresettingspluginmanager.cpp
    messageviewerconfigureplugins/messageviewerconfiguresettingspluginwidget.cpp
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletingplugin.cpp
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletingpluginmanager.cpp
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletingpluginwidget.cpp
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletinginterface.cpp
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletingparameters.cpp
    messagepartthemes/default/converthtmltoplaintext.cpp
    messagepartthemes/default/defaultrenderer.cpp
    messagepartthemes/default/htmlblock.cpp
    messagepartthemes/default/messagepartrenderermanager.cpp
    messagepartthemes/default/plugins/attachmentmessagepartrenderer.cpp
    messagepartthemes/default/plugins/messagepartrenderer.cpp
    messagepartthemes/default/plugins/textmessagepartrenderer.cpp
    messagepartthemes/default/plugins/quotehtml.cpp
    messagepartthemes/default/messagepartrenderbase.cpp
    messagepartthemes/default/messagepartrenderplugin.cpp
    messagepartthemes/default/messagepartrendererfactory.cpp
    interfaces/htmlwriter.cpp
    remote-content/remotecontentmanager.cpp
    remote-content/remotecontentinfo.cpp
    remote-content/remotecontentmenu.cpp
    remote-content/remotecontentconfiguredialog.cpp
    remote-content/remotecontentconfigurewidget.cpp
    remote-content/remotecontentdialog.cpp
    remote-content/remotecontentwidget.cpp
    remote-content/remotecontentstatustypecombobox.cpp
    settings/messageviewersettings.cpp
    messagepartthemes.qrc
    viewerplugins/viewerplugintoolmanager.h
    viewerplugins/viewerplugininterface.h
    viewerplugins/viewerplugin.h
    viewerplugins/viewerpluginmanager.h
    findbar/findbarsourceview.h
    job/modifymessagedisplayformatjob.h
    htmlwriter/webengineembedpart.h
    htmlwriter/webengineparthtmlwriter.h
    htmlwriter/bufferedhtmlwriter.h
    htmlwriter/filehtmlwriter.h
    utils/markmessagereadhandler.h
    utils/iconnamecache.h
    utils/messageviewerutil_p.h
    utils/messageviewerutil.h
    utils/mimetype.h
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletingpluginwidget.h
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletingparameters.h
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletingplugin.h
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletinginterface.h
    messageviewercheckbeforedeletingplugins/messageviewercheckbeforedeletingpluginmanager.h
    messageviewer_private_export.h
    header/plainheaderstyle.h
    header/contactdisplaymessagememento.h
    header/grantleeheaderstyle.h
    header/headerstyle_util.h
    header/headerstylemenumanager.h
    header/richheaderstrategy.h
    header/grantleeheaderformatter.h
    header/headerstyle.h
    header/headerstyleplugin.h
    header/headerstrategy.h
    header/headerstylepluginmanager.h
    header/kxface.h
    header/headerstyleinterface.h
    messageviewerconfigureplugins/messageviewerconfiguresettingspluginwidget.h
    messageviewerconfigureplugins/messageviewerconfiguresettingsplugin.h
    messageviewerconfigureplugins/messageviewerconfiguresettingspluginmanager.h
    messagepartthemes/default/converthtmltoplaintext.h
    messagepartthemes/default/htmlblock.h
    messagepartthemes/default/defaultrenderer.h
    messagepartthemes/default/messagepartrenderplugin.h
    messagepartthemes/default/messagepartrendererfactory_p.h
    messagepartthemes/default/messagepartrenderermanager.h
    messagepartthemes/default/messagepartrendererfactory.h
    messagepartthemes/default/plugins/textmessagepartrenderer.h
    messagepartthemes/default/plugins/messagepartrenderer.h
    messagepartthemes/default/plugins/quotehtml.h
    messagepartthemes/default/plugins/attachmentmessagepartrenderer.h
    messagepartthemes/default/defaultrenderer_p.h
    messagepartthemes/default/messagepartrendererbase.h
    interfaces/urlhandler.h
    interfaces/bodyparturlhandler.h
    interfaces/htmlwriter.h
    widgets/invitationsettings.h
    widgets/mailsourcewebengineviewer.h
    widgets/mailsourceviewtextbrowserwidget.h
    widgets/shownextmessagewidget.h
    widgets/htmlstatusbar.h
    widgets/printingsettings.h
    widgets/opensavedfilefolderwidget.h
    widgets/configurewidget.h
    widgets/vcardviewer.h
    widgets/attachmentdialog.h
    remote-content/remotecontentinfo.h
    remote-content/remotecontentconfigurewidget.h
    remote-content/remotecontentdialog.h
    remote-content/remotecontentstatustypecombobox.h
    remote-content/remotecontentmanager.h
    remote-content/remotecontentwidget.h
    remote-content/remotecontentmenu.h
    remote-content/remotecontentconfiguredialog.h
    scamdetection/scamcheckshorturlmanager.h
    scamdetection/scamdetectionwebengine.h
    scamdetection/scamdetectioninfo.h
    scamdetection/scamdetectionwarningwidget.h
    scamdetection/scamcheckshorturl.h
    scamdetection/scamdetectionwhitelistsettingsmanager.h
    scamdetection/scamexpandurljob.h
    scamdetection/scamattribute.h
    scamdetection/scamdetectiondetailsdialog.h
    viewer/messagedisplayformatattribute.h
    viewer/objecttreeviewersource.h
    viewer/viewer_p.h
    viewer/messagewidgetarea.h
    viewer/renderer/messageviewerrenderer.h
    viewer/urlhandlermanager.h
    viewer/urlhandlermanager_p.h
    viewer/stl_util.h
    viewer/objecttreeemptysource.h
    viewer/csshelperbase.h
    viewer/csshelper.h
    viewer/viewerpurposemenuwidget.h
    viewer/webengine/loadexternalreferencesurlinterceptor/loadexternalreferencesurlinterceptor.h
    viewer/webengine/cidreferencesurlinterceptor/cidreferencesurlinterceptor.h
    viewer/webengine/mailwebengineview.h
    viewer/webengine/cidschemehandler/cidschemehandler.h
    viewer/webengine/mailwebenginepage.h
    viewer/viewer.h
    viewer/editorwatcher.h
    viewer/attachmentstrategy.h
    viewer/mimeparttree/mimetreemodel.h
    viewer/mimeparttree/mimeparttreeview.h
    dkim-verify/dkimcheckpolicy.h
    dkim-verify/dkimauthenticationstatusinfoconverter.h
    dkim-verify/dkimkeyrecord.h
    dkim-verify/dkimauthenticationstatusinfoutil.h
    dkim-verify/dkimmanagerauthenticationserver.h
    dkim-verify/dkimviewermenu.h
    dkim-verify/dkimauthenticationstatusinfo.h
    dkim-verify/dkimcheckauthenticationstatusjob.h
    dkim-verify/dkimresultattribute.h
    dkim-verify/dmarcrecordjob.h
    dkim-verify/dkimmanagerkey.h
    dkim-verify/dkimstoreresultjob.h
    dkim-verify/dkimutil.h
    dkim-verify/dmarcmanager.h
    dkim-verify/dkimmanagerulesdialog.h
    dkim-verify/dkimmanagerkeydialog.h
    dkim-verify/dkimdownloadkeyjob.h
    dkim-verify/dkimchecksignaturejob.h
    dkim-verify/dkimmanagerulescombobox.h
    dkim-verify/dkimruledialog.h
    dkim-verify/dkimmanagerrules.h
    dkim-verify/dkiminfo.h
    dkim-verify/dkimcheckfulljob.h
    dkim-verify/dkimmanageruleswidget.h
    dkim-verify/dmarcinfo.h
    dkim-verify/dkimrulewidget.h
    dkim-verify/dkimgeneraterulejob.h
    dkim-verify/dkimmanager.h
    dkim-verify/dkimheaderparser.h
    dkim-verify/dkimwidgetinfo.h
    dkim-verify/dkimrule.h
    dkim-verify/dkimcheckpolicyjob.h
    dkim-verify/dkimmanagerkeywidget.h
    dkim-verify/dmarcpolicyjob.h
    antispam/spamheaderanalyzer.h
    antispam/antispamconfig.h
    settings/messageviewersettings.h

    openurlwith/openwithurlinfo.h
    openurlwith/openwithurlinfo.cpp
    openurlwith/openurlwithmanager.h
    openurlwith/openurlwithmanager.cpp
    openurlwith/openurlwithjob.h
    openurlwith/openurlwithjob.cpp

    mdn/mdnwarningwidget.cpp
    mdn/mdnwarningwidget.h
    )

ecm_qt_declare_logging_category(KF5MessageViewer HEADER messageviewer_debug.h IDENTIFIER MESSAGEVIEWER_LOG CATEGORY_NAME org.kde.pim.messageviewer
        DESCRIPTION "messagelib (messageviewer)"
        OLD_CATEGORY_NAMES log_messageviewer
        EXPORT MESSAGELIB
    )
ecm_qt_declare_logging_category(KF5MessageViewer HEADER messageviewer_dkimcheckerdebug.h IDENTIFIER MESSAGEVIEWER_DKIMCHECKER_LOG CATEGORY_NAME org.kde.pim.messageviewer_dkimchecker
        DESCRIPTION "messagelib (messageviewer dkim support)"
        EXPORT MESSAGELIB
    )


kconfig_add_kcfg_files(KF5MessageViewer
    settings/globalsettings_messageviewer.kcfgc
    )

ki18n_wrap_ui(KF5MessageViewer
    ui/settings.ui
    ui/invitationsettings.ui
    ui/printingsettings.ui
    )


if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5MessageViewer PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KF5MessageViewer BASE_NAME messageviewer)

if (${Inotify_FOUND})
    target_include_directories(KF5MessageViewer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/MessageViewer/;${Inotify_INCLUDE_DIRS}>")
else()
    target_include_directories(KF5MessageViewer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/MessageViewer/>")
endif()

target_link_libraries(KF5MessageViewer
    PUBLIC
    KF5::MessageCore
    KF5::PimCommon
    KF5::AkonadiCore
    KF5::AkonadiMime
    KF5::Contacts
    KF5::Libkleo
    KF5::MimeTreeParser
    KF5::Service
    PRIVATE
    KF5::SyntaxHighlighting
    KF5::ItemViews
    Qt::Network
    KF5::WebEngineViewer
    KF5::GrantleeTheme
    KF5::MailTransportAkonadi
    KF5::Mime
    KF5::Mbox
    KF5::PimTextEdit
    KF5::Gravatar
    KF5::IdentityManagement
    KF5::IconThemes
    KF5::I18n
    KF5::KIOFileWidgets
    KF5::KIOWidgets
    KF5::XmlGui
    KF5::AkonadiContact
    ${TEMPLATES_LIB}
    Qt::PrintSupport
    ${Inotify_LIBRARIES}
    qca-qt${QT_MAJOR_VERSION}
    KF5::Notifications
    )
if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(KF5MessageViewer PRIVATE QGpgmeQt6)
else()
    target_link_libraries(KF5MessageViewer PRIVATE QGpgme)
endif()
if (TARGET Qt6::Core5Compat)
    target_link_libraries(KF5MessageViewer PRIVATE Qt6::Core5Compat) 
endif()

set_target_properties(KF5MessageViewer PROPERTIES
    VERSION ${MESSAGEVIEWER_VERSION}
    SOVERSION ${MESSAGEVIEWER_SOVERSION}
    EXPORT_NAME MessageViewer
    )

install(TARGETS
    KF5MessageViewer
    EXPORT KF5MessageViewerTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
    )

ecm_generate_headers(MessageViewer_Camelcasewebengine_HEADERS
    HEADER_NAMES
    MailWebEnginePage
    MailWebEngineView

    REQUIRED_HEADERS MessageViewer_webengine_HEADERS
    PREFIX MessageViewer
    RELATIVE viewer/webengine
    )

ecm_generate_headers(MessageViewer_Camelcasescam_HEADERS
    HEADER_NAMES
    ScamExpandUrlJob
    ScamCheckShortUrlManager
    ScamCheckShortUrl
    ScamDetectionWhiteListSettingsManager
    ScamDetectionInfo
    REQUIRED_HEADERS MessageViewer_scam_HEADERS
    PREFIX MessageViewer
    RELATIVE scamdetection
    )

ecm_generate_headers(MessageViewer_Cameldkimverify_HEADERS
    HEADER_NAMES
    DKIMManagerKey
    DKIMCheckSignatureJob
    DKIMManager
    DKIMManagerKeyWidget
    DKIMManagerKeyDialog
    DKIMKeyRecord
    DKIMInfo
    DKIMWidgetInfo
    DKIMCheckPolicy
    DKIMManagerRules
    DKIMRule
    DKIMHeaderParser
    DKIMManagerAuthenticationServer
    DKIMViewerMenu
    DKIMManageRulesDialog
    DKIMManageRulesWidget
    REQUIRED_HEADERS MessageViewer_dkimverify_HEADERS
    PREFIX MessageViewer
    RELATIVE dkim-verify
)

ecm_generate_headers(MessageViewer_Camelremotecontent_HEADERS
    HEADER_NAMES
    RemoteContentConfigureDialog
    RemoteContentMenu
    REQUIRED_HEADERS MessageViewer_remotecontent_HEADERS
    PREFIX MessageViewer
    RELATIVE remote-content
    )

ecm_generate_headers(MessageViewer_Camelopenurlwith_HEADERS
    HEADER_NAMES
    OpenWithUrlInfo
    OpenUrlWithManager
    REQUIRED_HEADERS MessageViewer_openurlwith_HEADERS
    PREFIX MessageViewer
    RELATIVE openurlwith
    )


ecm_generate_headers(MessageViewer_Camelcaseviewer_HEADERS
    HEADER_NAMES
    AttachmentStrategy
    Viewer
    CSSHelperBase
    CSSHelper
    ObjectTreeEmptySource
    EditorWatcher
    Stl_Util
    REQUIRED_HEADERS MessageViewer_viewer_HEADERS
    PREFIX MessageViewer
    RELATIVE viewer
    )

ecm_generate_headers(MessageViewer_Camelcasewidgets_HEADERS
    HEADER_NAMES
    InvitationSettings
    PrintingSettings
    ConfigureWidget
    REQUIRED_HEADERS MessageViewer_widgets_HEADERS
    PREFIX MessageViewer
    RELATIVE widgets
    )

ecm_generate_headers(MessageViewer_Camelcaseutils_HEADERS
    HEADER_NAMES
    IconNameCache
    MarkMessageReadHandler
    MessageViewerUtil
    MimeType
    REQUIRED_HEADERS MessageViewer_utils_HEADERS
    PREFIX MessageViewer
    RELATIVE utils
    )

ecm_generate_headers(MessageViewer_Camelcaseantispam_HEADERS
    HEADER_NAMES
    SpamHeaderAnalyzer
    REQUIRED_HEADERS MessageViewer_antispam_HEADERS
    PREFIX MessageViewer
    RELATIVE antispam
    )

ecm_generate_headers(MessageViewer_Camelcaseinterfaces_HEADERS
    HEADER_NAMES
    HtmlWriter
    BodyPartURLHandler
    URLHandler
    REQUIRED_HEADERS MessageViewer_interfaces_HEADERS
    PREFIX MessageViewer
    RELATIVE interfaces
    )

ecm_generate_headers(MessageViewer_Camelcasehtmlwriter_HEADERS
    HEADER_NAMES
    BufferedHtmlWriter
    FileHtmlWriter
    REQUIRED_HEADERS MessageViewer_htmlwriter_HEADERS
    PREFIX MessageViewer
    RELATIVE htmlwriter
    )

ecm_generate_headers(MessageViewer_Camelcasesettings_HEADERS
    HEADER_NAMES
    MessageViewerSettings
    REQUIRED_HEADERS MessageViewer_settings_HEADERS
    PREFIX MessageViewer
    RELATIVE settings
    )

ecm_generate_headers(MessageViewer_CamelcaseConfigurePlugins_HEADERS
    HEADER_NAMES
    MessageViewerConfigureSettingsPluginManager
    MessageViewerConfigureSettingsPlugin
    MessageViewerConfigureSettingsPluginWidget
    REQUIRED_HEADERS MessageViewer_ConfigurePlugins_HEADERS
    PREFIX MessageViewer
    RELATIVE messageviewerconfigureplugins
    )

ecm_generate_headers(MessageViewer_CamelcaseCheckBeforeDeletingPlugin_HEADERS
    HEADER_NAMES
    MessageViewerCheckBeforeDeletingPlugin
    MessageViewerCheckBeforeDeletingPluginManager
    MessageViewerCheckBeforeDeletingPluginWidget
    MessageViewerCheckBeforeDeletingInterface
    MessageViewerCheckBeforeDeletingParameters
    REQUIRED_HEADERS MessageViewer_CheckBeforeDeletingPlugin_HEADERS
    PREFIX MessageViewer
    RELATIVE messageviewercheckbeforedeletingplugins
    )

ecm_generate_headers(MessageViewer_Camelcaseheader_HEADERS
    HEADER_NAMES
    HeaderStrategy
    GrantleeHeaderTestStyle
    GrantleeHeaderStyle
    HeaderStyle
    KXFace
    HeaderStyle_Util
    HeaderStylePlugin
    HeaderStyleInterface
    PlainHeaderStyle
    RichHeaderStrategy
    HeaderStylePluginManager
    HeaderStyleMenuManager
    REQUIRED_HEADERS MessageViewer_header_HEADERS
    PREFIX MessageViewer
    RELATIVE header
    )

ecm_generate_headers(MessageViewer_Camelcaseviewerplugin_HEADERS
    HEADER_NAMES
    ViewerPluginManager
    ViewerPlugin
    ViewerPluginInterface
    ViewerPluginToolManager
    REQUIRED_HEADERS MessageViewer_viewerplugin_HEADERS
    PREFIX MessageViewer
    RELATIVE viewerplugins
    )

ecm_generate_headers(MessageViewer_Camelcasemdnwarningwidget_HEADERS
    HEADER_NAMES
    MDNWarningWidget
    REQUIRED_HEADERS MessageViewer_mdnwarningwidget_HEADERS
    PREFIX MessageViewer
    RELATIVE mdn
    )

ecm_generate_headers(MessageViewer_Camelcaserenderer_HEADERS
    HEADER_NAMES
    HtmlBlock
    MessagePartRendererBase
    MessagePartRendererManager
    MessagePartRenderPlugin
    REQUIRED_HEADERS MessageViewer_renderer_HEADERS
    PREFIX MessageViewer
    RELATIVE messagepartthemes/default
    )

ecm_generate_pri_file(BASE_NAME MessageViewer
    LIB_NAME KF5MessageViewer
    DEPS "PimCommon MessageCore AkonadiCore AkonadiMime KContacts Libkleo MimeTreeParser" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/MessageViewer
    )
install(FILES
    ${MessageViewer_Cameldkimverify_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/MessageViewer/MessageViewer
    COMPONENT Devel
)

install(FILES
    ${MessageViewer_dkimverify_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/MessageViewer/messageviewer
    COMPONENT Devel
)


install(FILES
    ${MessageViewer_Camelcasemdnwarningwidget_HEADERS}
    ${MessageViewer_CamelcaseCheckBeforeDeletingPlugin_HEADERS}
    ${MessageViewer_CamelcaseConfigurePlugins_HEADERS}
    ${MessageViewer_Camelcasewebengine_HEADERS}
    ${MessageViewer_Camelcaseheader_HEADERS}
    ${MessageViewer_Camelcaseviewerplugin_HEADERS}
    ${MessageViewer_Camelcasesettings_HEADERS}
    ${MessageViewer_Camelcaseutils_HEADERS}
    ${MessageViewer_Camelcaseinterfaces_HEADERS}
    ${MessageViewer_Camelcasehtmlwriter_HEADERS}
    ${MessageViewer_Camelcaseviewer_HEADERS}
    ${MessageViewer_Camelremotecontent_HEADERS}
    ${MessageViewer_Camelcasewidgets_HEADERS}
    ${MessageViewer_Camelcaseantispam_HEADERS}
    ${MessageViewer_Camelfindbar_HEADERS}
    ${MessageViewer_Camelcasescam_HEADERS}
    ${MessageViewer_Camelcaserenderer_HEADERS}
    ${MessageViewer_Camelopenurlwith_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/MessageViewer/MessageViewer
    COMPONENT Devel
    )

install(FILES
    ${MessageViewer_mdnwarningwidget_HEADERS}
    ${MessageViewer_CheckBeforeDeletingPlugin_HEADERS}
    ${MessageViewer_ConfigurePlugins_HEADERS}
    ${MessageViewer_webengine_HEADERS}
    ${MessageViewer_scam_HEADERS}
    ${MessageViewer_viewerplugin_HEADERS}
    ${MessageViewer_settings_HEADERS}
    ${MessageViewer_header_HEADERS}
    ${MessageViewer_utils_HEADERS}
    ${MessageViewer_interfaces_HEADERS}
    ${MessageViewer_htmlwriter_HEADERS}
    ${MessageViewer_HEADERS}
    ${MessageViewer_viewer_HEADERS}
    ${MessageViewer_widgets_HEADERS}
    ${MessageViewer_antispam_HEADERS}
    ${MessageViewer_findbar_HEADERS}
    ${MessageViewer_renderer_HEADERS}
    ${MessageViewer_remotecontent_HEADERS}
    ${MessageViewer_openurlwith_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messageviewer_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/globalsettings_messageviewer.h
    ${CMAKE_CURRENT_BINARY_DIR}/messageviewer_debug.h
    ${CMAKE_CURRENT_BINARY_DIR}/config-messageviewer.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/MessageViewer/messageviewer
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


install(FILES header/data/messageviewer_header_themes.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR} )

install(FILES notify/messageviewer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFYRCDIR} )
install(FILES scamdetection/data/longurlServices.json DESTINATION ${KDE_INSTALL_DATADIR}/messageviewer )

if (BUILD_QCH)
    ecm_add_qch(
        KF5MessageViewer_QCH
        NAME KF5MessageViewer
        BASE_NAME KF5MessageViewer
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${MessageViewer_CheckBeforeDeletingPlugin_HEADERS}
        ${MessageViewer_ConfigurePlugins_HEADERS}
        ${MessageViewer_webengine_HEADERS}
        ${MessageViewer_scam_HEADERS}
        ${MessageViewer_viewerplugin_HEADERS}
        ${MessageViewer_settings_HEADERS}
        ${MessageViewer_header_HEADERS}
        ${MessageViewer_utils_HEADERS}
        ${MessageViewer_interfaces_HEADERS}
        ${MessageViewer_htmlwriter_HEADERS}
        ${MessageViewer_HEADERS}
        ${MessageViewer_viewer_HEADERS}
        ${MessageViewer_widgets_HEADERS}
        ${MessageViewer_antispam_HEADERS}
        ${MessageViewer_findbar_HEADERS}
        ${MessageViewer_renderer_HEADERS}
        ${MessageViewer_remotecontent_HEADERS}
        LINK_QCHS
            Qt5Core_QCH
            Qt5Gui_QCH
            Qt5Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            MESSAGEVIEWER_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )

    ecm_install_qch_export(
        TARGETS KF5MessageViewer_QCH
        FILE KF5MessageViewerQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5MessageViewerQchTargets.cmake\")")
endif()
