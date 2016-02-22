

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )


add_definitions(-DTRANSLATION_DOMAIN=\"libmessageviewer\")

# KCFG files:
# The main messageviewer.kcfg is configured by CMake and put in the build directory.

if(KDEPIM_ENTERPRISE_BUILD)
    set(LEGACY_MANGLE_FROM_TO_HEADERS true)
    set(LEGACY_BODY_INVITES true)
    set(EXCHANGE_COMPATIBLE_INVITATIONS true)
    set(OUTLOOK_COMPATIBLE_INVITATION_REPLY_COMMENTS true)
else()
    set(LEGACY_MANGLE_FROM_TO_HEADERS false)
    set(LEGACY_BODY_INVITES false)
    set(EXCHANGE_COMPATIBLE_INVITATIONS false)
    set(OUTLOOK_COMPATIBLE_INVITATION_REPLY_COMMENTS false)
endif()

configure_file(settings/messageviewer.kcfg.cmake ${CMAKE_CURRENT_BINARY_DIR}/messageviewer.kcfg)
include(CheckIncludeFiles)
check_include_files(sys/inotify.h SYS_INOTIFY_H_FOUND)
macro_bool_to_01(SYS_INOTIFY_H_FOUND HAVE_SYS_INOTIFY_H)
configure_file(config-messageviewer.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-messageviewer.h)

# target_include_directories does not handle empty include paths
include_directories(${GPGME_INCLUDES})

if(BUILD_TESTING)
    add_subdirectory(scamdetection/autotests)
    add_subdirectory(findbar/autotests)
    add_subdirectory(scamdetection/tests)
    add_subdirectory(adblock/tests/)
    add_subdirectory(viewerplugins/tests/)
endif()
add_subdirectory(pics)
add_subdirectory(kconf_update)
add_subdirectory(about)

if(DEBUG_SIGNATURE)
    add_definitions(-DDEBUG_SIGNATURE)
endif()

set(_mailwebview_cpp 
    adblock/adblockelementhiding.cpp
    adblock/adblockmanager.cpp
    adblock/adblockrulefallbackimpl.cpp
    adblock/adblockblockableitemsdialog.cpp
    adblock/adblockblockableitemswidget.cpp

    adblock/common/adblockrule.cpp
    adblock/common/adblockruletextmatchimpl.cpp
    adblock/common/adblockrulenullimpl.cpp
    adblock/common/adblocksettingwidget.cpp
    adblock/common/adblockutil.cpp
    adblock/common/adblockcreatefilterdialog.cpp
    adblock/common/adblockaddsubscriptiondialog.cpp
    adblock/common/adblocklistwidget.cpp
    adblock/common/adblocksyntaxhighlighter.cpp
    adblock/common/adblockshowlistdialog.cpp
    adblock/common/adblockhostmatcher.cpp
    )


# For the moment we can't replace mailwebengine
set(libmessageviewer_mailviewer_SRCS
    viewer/webview/mailwebview.cpp
    viewer/webview/webpage.cpp
    viewer/webview/networkaccessmanager.cpp
    )
set(libmessageviewer_web_LIB KF5::WebKit Qt5::WebKitWidgets)

if (QTWEBENGINE_EXPERIMENTAL_OPTION)
    set(libmessageviewer_mailviewer_SRCS
        viewer/webengine/mailwebengineview.cpp
        viewer/webengine/mailwebenginepage.cpp
        viewer/webengine/mailwebengineaccesskey.cpp
        ${libmessageviewer_mailviewer_SRCS}
        findbar/findbarwebengineview.cpp
        )

    set(libmessageviewer_webengine_SRCS
        webengine/networkmanager/networkaccessmanagerwebengine.cpp
        webengine/urlinterceptor/networkurlinterceptor.cpp
        webengine/urlinterceptor/networkpluginurlinterceptorinterface.cpp
        webengine/urlinterceptor/networkurlinterceptorpluginmanager.cpp
        webengine/urlinterceptor/networkpluginurlinterceptor.cpp
        webengine/urlinterceptor/networkurlinterceptormanager.cpp
        webengine/urlinterceptor/networkpluginurlinterceptorconfigurewidget.cpp
        webengine/webhittestresult.cpp
        )
    set(libmessageviewer_web_LIB Qt5::WebEngine Qt5::WebEngineWidgets ${libmessageviewer_web_LIB})
endif()

set(libmessageviewer_otp_SRCS
    interfaces/bodypartformatter.cpp
    viewer/bodypartformatter.cpp
    viewer/bodypartformatterfactory.cpp
    viewer/cryptohelper.cpp
    viewer/nodehelper.cpp
    viewer/objecttreeparser.cpp
    viewer/messagepart.cpp
    viewer/partnodebodypart.cpp
    viewer/pluginloaderbase.cpp
    viewer/memento/cryptobodypartmemento.cpp
    viewer/memento/decryptverifybodypartmemento.cpp
    viewer/memento/verifydetachedbodypartmemento.cpp
    viewer/memento/verifyopaquebodypartmemento.cpp
    )

set(libmessageviewer_otp_extra_SRCS
    job/kleojobexecutor.cpp
    #for quotes
    viewer/csshelperbase.cpp
    viewer/attachmentstrategy.cpp
    utils/iconnamecache.cpp
    utils/mimetype.cpp
    viewer/converthtmltoplaintext.cpp
    viewer/urlhandlermanager_otp.cpp
    #HTML Writer
    htmlwriter/filehtmlwriter.cpp
    htmlwriter/queuehtmlwriter.cpp
    )

set(libmessageviewer_viewer_SRCS
    viewer/attachmentstrategy.cpp
    viewer/bodypartformatter.cpp
    viewer/bodypartformatterfactory.cpp
    viewer/cryptohelper.cpp
    viewer/csshelper.cpp
    viewer/csshelperbase.cpp
    viewer/editorwatcher.cpp
    viewer/mimeparttree/mimetreemodel.cpp
    viewer/nodehelper.cpp
    viewer/objecttreeemptysource.cpp
    viewer/objecttreeparser.cpp
    viewer/objecttreeviewersource.cpp
    viewer/messagepart.cpp
    viewer/partnodebodypart.cpp
    viewer/pluginloaderbase.cpp
    viewer/viewer.cpp
    viewer/viewer_p.cpp
    viewer//webview/webviewaccesskey.cpp
    viewer/messagedisplayformatattribute.cpp
    viewer/urlhandlermanager.cpp
    viewer/mimeparttree/mimeparttreeview.cpp
    viewer/converthtmltoplaintext.cpp
    viewer/memento/cryptobodypartmemento.cpp
    viewer/memento/decryptverifybodypartmemento.cpp
    viewer/memento/verifydetachedbodypartmemento.cpp
    viewer/memento/verifyopaquebodypartmemento.cpp
    )

set(libmessageviewer_widgets_SRCS
    widgets/attachmentdialog.cpp
    widgets/configurewidget.cpp
    widgets/printingsettings.cpp
    widgets/htmlstatusbar.cpp
    widgets/vcardviewer.cpp
    widgets/invitationsettings.cpp
    widgets/openattachmentfolderwidget.cpp
    widgets/mailsourceviewer.cpp
    widgets/mailsourceviewtextbrowserwidget.cpp
    widgets/zoomactionmenu.cpp
    )

set(libmessageviewer_header_SRCS
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
    )

set(libmessageviewer_scamdetection_SRCS
    scamdetection/scamdetectionwarningwidget.cpp
    scamdetection/scamdetection.cpp
    scamdetection/scamdetectiondetailsdialog.cpp
    scamdetection/scamattribute.cpp
    scamdetection/scamcheckshorturl.cpp
    )

set(libmessageviewer_findbar_SRCS
    findbar/findbarbase.cpp
    findbar/findbarwebview.cpp
    findbar/findbarsourceview.cpp
    )

set(libmessageviewer_utils_SRCS
    utils/iconnamecache.cpp
    utils/markmessagereadhandler.cpp
    utils/messageviewerutil.cpp
    utils/mimetype.cpp
    )

set(libmessageviewer_htmlwriter_SRCS
    htmlwriter/filehtmlwriter.cpp
    htmlwriter/queuehtmlwriter.cpp
    htmlwriter/teehtmlwriter.cpp
    htmlwriter/webkitparthtmlwriter.cpp
    )

set(libmessageviewer_antispam_SRCS
    antispam/spamheaderanalyzer.cpp
    antispam/antispamconfig.cpp
    )

set(libmessageviewer_job_SRCS
    job/createtodojob.cpp
    job/createeventjob.cpp
    job/createnotejob.cpp
    job/attachmenteditjob.cpp
    job/modifymessagedisplayformatjob.cpp
    job/kleojobexecutor.cpp
    )

set(libmessageviewer_viewerplugins_SRCS
    viewerplugins/viewerpluginmanager.cpp
    viewerplugins/viewerplugin.cpp
    viewerplugins/viewerplugininterface.cpp
    viewerplugins/viewerplugintoolmanager.cpp
    )

set(libmessageviewer_SRCS
    ${libmessageviewer_webengine_SRCS}
    ${libmessageviewer_viewer_SRCS}
    ${libmessageviewer_widgets_SRCS}
    ${libmessageviewer_header_SRCS}
    ${libmessageviewer_scamdetection_SRCS}
    ${libmessageviewer_findbar_SRCS}
    ${libmessageviewer_utils_SRCS}
    ${libmessageviewer_htmlwriter_SRCS}
    ${libmessageviewer_antispam_SRCS}
    ${libmessageviewer_job_SRCS}
    ${libmessageviewer_viewerplugins_SRCS}
    settings/messageviewersettings.cpp
    interfaces/bodypartformatter.cpp
    ${_mailwebview_cpp}
    ${libmessageviewer_mailviewer_SRCS}
    )

ecm_qt_declare_logging_category(libmessageviewer_SRCS HEADER messageviewer_debug.h IDENTIFIER MESSAGEVIEWER_LOG CATEGORY_NAME log_messageviewer)

kconfig_add_kcfg_files(libmessageviewer_SRCS
    settings/globalsettings_messageviewer.kcfgc
    )

ki18n_wrap_ui(libmessageviewer_SRCS
    ui/settings.ui
    ui/invitationsettings.ui
    ui/printingsettings.ui
    adblock/ui/settings_adblock.ui
    adblock/ui/adblockcreatefilterwidget.ui
    )


add_library(KF5MessageViewer ${libmessageviewer_SRCS})
generate_export_header(KF5MessageViewer BASE_NAME messageviewer)
add_library(KF5::MessageViewer ALIAS KF5MessageViewer)

target_include_directories(KF5MessageViewer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/MessageViewer/;${KDE_INSTALL_INCLUDEDIR_KF5}/messageviewer>")
target_include_directories(KF5MessageViewer PUBLIC "$<BUILD_INTERFACE:${MessageViewer_SOURCE_DIR}/src;${MessageViewer_BINARY_DIR}/src;>")

target_link_libraries(KF5MessageViewer
    PUBLIC
    KF5::MessageCore
    KF5::PimCommon
    KF5::AkonadiCore
    KF5::AkonadiMime
    KF5::Contacts
    KF5::Libkleo
    PRIVATE
    KF5::ItemViews
    ${libmessageviewer_web_LIB}
    KF5::Libkdepim
    KF5::GrantleeTheme
    KF5::KaddressbookGrantlee
    Grantlee5::TextDocument
    Grantlee5::Templates
    KF5::MailTransport
    KF5::Mime
    KF5::Mbox
    KF5::AkonadiNotes
    KF5::PimTextEdit
    KF5::CalendarCore
    KF5::Gravatar
    KF5::IconThemes
    )
if (WIN32)
    target_link_libraries(KF5MessageViewer PUBLIC KF5::Gpgmepp)
else()
    target_link_libraries(KF5MessageViewer PUBLIC KF5::Gpgmepp-pthread)
endif()

target_include_directories(KF5MessageViewer PUBLIC $<TARGET_PROPERTY:KF5::WebKit,INTERFACE_INCLUDE_DIRECTORIES>)

set_target_properties(KF5MessageViewer PROPERTIES
    VERSION ${MESSAGEVIEWER_VERSION_STRING}
    SOVERSION ${MESSAGEVIEWER_SOVERSION}
    EXPORT_NAME MessageViewer
    )

install(TARGETS
    KF5MessageViewer
    EXPORT KF5MessageViewerTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK}
    )

ecm_generate_headers(MessageViewer_Camelcasewebviewer_HEADERS
    HEADER_NAMES
    MailWebView
    WebViewAccessKey
    WebPage
    REQUIRED_HEADERS MessageViewer_webviewer_HEADERS
    PREFIX MessageViewer
    RELATIVE viewer/webview
    )

if (QTWEBENGINE_EXPERIMENTAL_OPTION)
    ecm_generate_headers(MessageViewer_Camelcasewebengine_HEADERS
        HEADER_NAMES
        MailWebEnginePage
        MailWebEngineAccessKey
        MailWebEngineView

        REQUIRED_HEADERS MessageViewer_webengine_HEADERS
        PREFIX MessageViewer
        RELATIVE viewer/webengine
        )
    ecm_generate_headers(MessageViewer_Camelcasewebengine_urlinterceptor_HEADERS
        HEADER_NAMES
        NetworkUrlInterceptorPluginManager
        NetworkUrlInterceptor
        NetworkPluginUrlInterceptorInterface
        NetworkPluginUrlInterceptor
        NetworkPluginUrlInterceptorConfigureWidget

        REQUIRED_HEADERS MessageViewer_webengine_urlinterceptor_HEADERS
        PREFIX MessageViewer
        RELATIVE webengine/urlinterceptor
        )
    ecm_generate_headers(MessageViewer_Camelcasewebengine_manager_HEADERS
        HEADER_NAMES
        NetworkAccessManagerWebEngine
        REQUIRED_HEADERS MessageViewer_webengine_manager_HEADERS
        PREFIX MessageViewer
        RELATIVE webengine/networkmanager
        )
    ecm_generate_headers(MessageViewer_Camelcasewebengine_misc_HEADERS
        HEADER_NAMES
        WebHitTestResult
        REQUIRED_HEADERS MessageViewer_webengine_misc_HEADERS
        PREFIX MessageViewer
        RELATIVE webengine/
	)
endif()

ecm_generate_headers(MessageViewer_Camelcaseviewer_HEADERS
    HEADER_NAMES
    Viewer
    CSSHelper
    PartMetaData
    ObjectTreeParser
    ObjectTreeEmptySource
    AttachmentStrategy
    NodeHelper
    EditorWatcher
    Stl_Util
    CSSHelperBase
    MessagePart
    Enums
    REQUIRED_HEADERS MessageViewer_viewer_HEADERS
    PREFIX MessageViewer
    RELATIVE viewer
    )

ecm_generate_headers(MessageViewer_Camelcasemain_HEADERS
    HEADER_NAMES
    ObjectTreeSourceIf
    REQUIRED_HEADERS MessageViewer_main_HEADERS
    PREFIX MessageViewer
    )


ecm_generate_headers(MessageViewer_Camelcasewidgets_HEADERS
    HEADER_NAMES
    InvitationSettings
    PrintingSettings
    ConfigureWidget
    ZoomActionMenu
    REQUIRED_HEADERS MessageViewer_widgets_HEADERS
    PREFIX MessageViewer
    RELATIVE widgets
    )

ecm_generate_headers(MessageViewer_Camelcaseutils_HEADERS
    HEADER_NAMES
    MessageViewerUtil
    MarkMessageReadHandler
    IconNameCache
    MimeType
    REQUIRED_HEADERS MessageViewer_utils_HEADERS
    PREFIX MessageViewer
    RELATIVE utils
    )

ecm_generate_headers(MessageViewer_Camelfindbar_HEADERS
    HEADER_NAMES
    FindBarWebView
    FindBarBase
    REQUIRED_HEADERS MessageViewer_findbar_HEADERS
    PREFIX MessageViewer
    RELATIVE findbar
    )

if (QTWEBENGINE_EXPERIMENTAL_OPTION)
    ecm_generate_headers(MessageViewer_CamelfindbarWebEngine_HEADERS
        HEADER_NAMES
        FindBarWebEngineView
        REQUIRED_HEADERS MessageViewer_findbarWebEngine_HEADERS
        PREFIX MessageViewer
        RELATIVE findbar
        )
endif()

ecm_generate_headers(MessageViewer_Camelcaseantispam_HEADERS
    HEADER_NAMES
    SpamHeaderAnalyzer
    REQUIRED_HEADERS MessageViewer_antispam_HEADERS
    PREFIX MessageViewer
    RELATIVE antispam
    )

ecm_generate_headers(MessageViewer_Camelcaseinterfaces_HEADERS
    HEADER_NAMES
    BodyPartFormatter
    BodyPart
    BodyPartURLHandler
    HtmlWriter
    URLHandler
    REQUIRED_HEADERS MessageViewer_interfaces_HEADERS
    PREFIX MessageViewer
    RELATIVE interfaces
    )

ecm_generate_headers(MessageViewer_Camelcasesettings_HEADERS
    HEADER_NAMES
    MessageViewerSettings
    REQUIRED_HEADERS MessageViewer_settings_HEADERS
    PREFIX MessageViewer
    RELATIVE settings
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

ecm_generate_headers(MessageViewer_Camelcasehtmlwriter_HEADERS
    HEADER_NAMES
    WebKitPartHtmlWriter
    FileHtmlWriter
    REQUIRED_HEADERS MessageViewer_htmlwriter_HEADERS
    PREFIX MessageViewer
    RELATIVE htmlwriter
    )

ecm_generate_headers(MessageViewer_Camelcaseadblockcommon_HEADERS
    HEADER_NAMES
    AdBlockListWidget
    AdBlockSettingWidget
    REQUIRED_HEADERS MessageViewer_adblockcommon_HEADERS
    PREFIX MessageViewer
    RELATIVE adblock/common
    )


ecm_generate_headers(MessageViewer_Camelcaseadblock_HEADERS
    HEADER_NAMES
    AdBlockBlockableItemsDialog
    AdBlockManager
    REQUIRED_HEADERS MessageViewer_adblock_HEADERS
    PREFIX MessageViewer
    RELATIVE adblock
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

ecm_generate_headers(MessageViewer_Camelcaseviewerjob_HEADERS
    HEADER_NAMES
    CreateTodoJob
    CreateNoteJob
    CreateEventJob
    REQUIRED_HEADERS MessageViewer_viewerjob_HEADERS
    PREFIX MessageViewer
    RELATIVE job
    )

ecm_generate_pri_file(BASE_NAME MessageViewer
    LIB_NAME KF5MessageViewer
    DEPS "PimCommon MessageCore AkonadiCore AkonadiMime Contacts Libkleo" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageViewer
    )


install(FILES
    ${MessageViewer_Camelcasewebengine_misc_HEADERS}
    ${MessageViewer_Camelcasewebengine_manager_HEADERS}
    ${MessageViewer_Camelcasewebengine_urlinterceptor_HEADERS}
    ${MessageViewer_Camelcasewebengine_HEADERS}
    ${MessageViewer_CamelfindbarWebEngine_HEADERS}
    ${MessageViewer_Camelcasewebviewer_HEADERS}
    ${MessageViewer_Camelcaseheader_HEADERS}
    ${MessageViewer_Camelcaseviewerplugin_HEADERS}
    ${MessageViewer_Camelcasesettings_HEADERS}
    ${MessageViewer_Camelcaseadblock_HEADERS}
    ${MessageViewer_Camelcasehtmlwriter_HEADERS}
    ${MessageViewer_Camelcaseutils_HEADERS}
    ${MessageViewer_Camelcaseinterfaces_HEADERS}
    ${MessageViewer_Camelcaseviewer_HEADERS}
    ${MessageViewer_Camelcasewidgets_HEADERS}
    ${MessageViewer_Camelcasemain_HEADERS}
    ${MessageViewer_Camelcaseviewerjob_HEADERS}
    ${MessageViewer_Camelcaseantispam_HEADERS}
    ${MessageViewer_Camelfindbar_HEADERS}
    ${MessageViewer_Camelcaseadblockcommon_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageViewer
    COMPONENT Devel
    )

install(FILES
    ${MessageViewer_webengine_misc_HEADERS}
    ${MessageViewer_webengine_manager_HEADERS}
    ${MessageViewer_webengine_urlinterceptor_HEADERS}
    ${MessageViewer_webengine_HEADERS}
    ${MessageViewer_findbarWebEngine_HEADERS}
    ${MessageViewer_adblockcommon_HEADERS}
    ${MessageViewer_webviewer_HEADERS}
    ${MessageViewer_viewerplugin_HEADERS}
    ${MessageViewer_settings_HEADERS}
    ${MessageViewer_adblock_HEADERS}
    ${MessageViewer_htmlwriter_HEADERS}
    ${MessageViewer_header_HEADERS}
    ${MessageViewer_utils_HEADERS}
    ${MessageViewer_interfaces_HEADERS}
    ${MessageViewer_HEADERS}
    ${MessageViewer_viewer_HEADERS}
    ${MessageViewer_main_HEADERS}
    ${MessageViewer_widgets_HEADERS}
    ${MessageViewer_viewerjob_HEADERS}
    ${MessageViewer_antispam_HEADERS}
    ${MessageViewer_findbar_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messageviewer_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/globalsettings_messageviewer.h
    ${CMAKE_CURRENT_BINARY_DIR}/messageviewer_debug.h
    ${CMAKE_CURRENT_BINARY_DIR}/ui_settings_adblock.h

    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/messageviewer
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


install(FILES header/data/messageviewer_header_themes.knsrc DESTINATION ${KDE_INSTALL_CONFDIR} )

install(FILES notify/messageviewer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR} )
install(FILES scamdetection/data/longurlServices.json DESTINATION ${KDE_INSTALL_DATADIR}/messageviewer )

Project(Otp)

set(libotp_SRCS
    ${libmessageviewer_otp_SRCS}
    ${libmessageviewer_otp_extra_SRCS}
    )
ecm_qt_declare_logging_category(libotp_SRCS HEADER messageviewer_debug.h IDENTIFIER MESSAGEVIEWER_LOG CATEGORY_NAME log_messageviewer)

add_library(KF5Otp
    ${libotp_SRCS}
    )
generate_export_header(KF5Otp BASE_NAME otp)
add_library(KF5::Otp ALIAS KF5Otp)

target_link_libraries(KF5Otp
    PUBLIC
    KF5::MessageCore #NodeHelper, StringUtils, Settings
    KF5::PimCommon #AttachmentTemporaryFile handling
    KF5::Libkleo
    PRIVATE
    KF5::Mime
    KF5::IconThemes
    ${libmessageviewer_web_LIB}
    Grantlee5::TextDocument
    )

install(TARGETS
    KF5Otp
    EXPORT KF5OtpTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK}
    )

set_target_properties(KF5Otp PROPERTIES
    VERSION ${MESSAGEVIEWER_VERSION_STRING}
    SOVERSION ${MESSAGEVIEWER_SOVERSION}
    EXPORT_NAME Otp
    )
