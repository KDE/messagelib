

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )


add_definitions(-DTRANSLATION_DOMAIN=\"libmessageviewer\")

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
check_include_files(sys/inotify.h SYS_INOTIFY_H_FOUND)
macro_bool_to_01(SYS_INOTIFY_H_FOUND HAVE_SYS_INOTIFY_H)
configure_file(config-messageviewer.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-messageviewer.h)

# target_include_directories does not handle empty include paths
include_directories(${GPGME_INCLUDES})

if(BUILD_TESTING)
    add_subdirectory(scamdetection/autotests)
    add_subdirectory(scamdetection/tests)
    add_subdirectory(viewerplugins/tests/)
    add_subdirectory(htmlwriter/autotests/)
    add_subdirectory(viewer/webengine/tests)
    add_subdirectory(messageviewerheaderplugins/autotests)
    add_subdirectory(messagepartthemes/default/autotests)
    add_subdirectory(viewer/webengine/autotests/)
endif()
add_subdirectory(pics)
add_subdirectory(kconf_update)
add_subdirectory(about)
add_subdirectory(messageviewerheaderplugins)
if(DEBUG_SIGNATURE)
    add_definitions(-DDEBUG_SIGNATURE)
endif()

set(libmessageviewer_mailviewer_SRCS
    viewer/webengine/mailwebengineview.cpp
    viewer/webengine/mailwebenginepage.cpp
    viewer/webengine/loadexternalreferencesurlinterceptor/loadexternalreferencesurlinterceptor.cpp
    viewer/webengine/cidreferencesurlinterceptor/cidreferencesurlinterceptor.cpp
    viewer/webengine/mailwebenginescript.cpp
    )

set(libmessageviewer_viewer_SRCS
    viewer/bodypartformatterfactory.cpp
    viewer/bodypartformatterfactorysingleton.cpp
    viewer/csshelper.cpp
    viewer/csshelperbase.cpp
    viewer/editorwatcher.cpp
    viewer/objecttreeemptysource.cpp
    viewer/objecttreeviewersource.cpp
    viewer/pluginloaderbase.cpp
    viewer/viewer.cpp
    viewer/viewer_p.cpp
    viewer/messagedisplayformatattribute.cpp
    viewer/urlhandlermanager.cpp
    viewer/mimeparttree/mimeparttreeview.cpp
    viewer/mimeparttree/mimetreemodel.cpp
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
    )

set(libmessageviewer_widgets_webengine_SRCS
    widgets/mailsourcewebengineviewer.cpp
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
    scamdetection/scamdetectiondetailsdialog.cpp
    scamdetection/scamattribute.cpp
    scamdetection/scamcheckshorturl.cpp
    scamdetection/scamexpandurljob.cpp
    scamdetection/scamcheckshorturlmanager.cpp
    )

set(libmessageviewer_scamdetection_webengine_SRCS
    scamdetection/scamdetectionwebengine.cpp
    )

set(libmessageviewer_findbar_SRCS
    findbar/findbarsourceview.cpp
    )

set(libmessageviewer_utils_SRCS
    utils/iconnamecache.cpp
    utils/markmessagereadhandler.cpp
    utils/messageviewerutil.cpp
    utils/mimetype.cpp
    )

set(libmessageviewer_htmlwriter_webengine_SRCS
    htmlwriter/webengineparthtmlwriter.cpp
    htmlwriter/webengineembedpart.cpp
    )

set(libmessageviewer_htmlwriter_SRCS
    htmlwriter/teehtmlwriter.cpp
    ${libmessageviewer_htmlwriter_webengine_SRCS}
    )

set(libmessageviewer_antispam_SRCS
    antispam/spamheaderanalyzer.cpp
    antispam/antispamconfig.cpp
    )

set(libmessageviewer_job_SRCS
    job/attachmenteditjob.cpp
    job/modifymessagedisplayformatjob.cpp
    )

set(libmessageviewer_viewerplugins_SRCS
    viewerplugins/viewerpluginmanager.cpp
    viewerplugins/viewerplugin.cpp
    viewerplugins/viewerplugininterface.cpp
    viewerplugins/viewerplugintoolmanager.cpp
    )

set(libmessageviewer_messagepartthemes_default_SRCS
    messagepartthemes/default/converthtmltoplaintext.cpp
    messagepartthemes/default/defaultrenderer.cpp
    messagepartthemes/default/htmlblock.cpp
    messagepartthemes/default/messagepartrenderermanager.cpp
    )

set(libmessageviewer_messagepartthemes_SRCS
    ${libmessageviewer_messagepartthemes_default_SRCS}
    )
set(libmessageviewer_SRCS
    ${libmessageviewer_messagepartthemes_SRCS}
    ${libmessageviewer_scamdetection_webengine_SRCS}
    ${libmessageviewer_widgets_webengine_SRCS}
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
    ${libmessageviewer_mailviewer_SRCS}
    )

qt5_add_resources(libmessageviewer_SRCS messagepartthemes.qrc)

ecm_qt_declare_logging_category(libmessageviewer_SRCS HEADER messageviewer_debug.h IDENTIFIER MESSAGEVIEWER_LOG CATEGORY_NAME log_messageviewer)

kconfig_add_kcfg_files(libmessageviewer_SRCS
    settings/globalsettings_messageviewer.kcfgc
    )

ki18n_wrap_ui(libmessageviewer_SRCS
    ui/settings.ui
    ui/invitationsettings.ui
    ui/printingsettings.ui
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
    KF5::MimeTreeParser
    PRIVATE
    KF5::ItemViews
    Qt5::Network
    Qt5::WebKitWidgets
    KF5::WebEngineViewer
    KF5::Libkdepim
    KF5::GrantleeTheme
    KF5::KaddressbookGrantlee
    Grantlee5::Templates
    KF5::MailTransport
    KF5::Mime
    KF5::Mbox
    KF5::AkonadiNotes
    KF5::PimTextEdit
    KF5::CalendarCore
    KF5::Gravatar
    KF5::IconThemes
    Grantlee5::TextDocument
    Grantlee5::Templates
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
    REQUIRED_HEADERS MessageViewer_scam_HEADERS
    PREFIX MessageViewer
    RELATIVE scamdetection
    )


ecm_generate_headers(MessageViewer_Camelcaseviewer_HEADERS
    HEADER_NAMES
    Viewer
    CSSHelperBase
    CSSHelper
    ObjectTreeEmptySource
    EditorWatcher
    Stl_Util
    BodyPartFormatterFactory
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
    BodyPartURLHandler
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

ecm_generate_pri_file(BASE_NAME MessageViewer
    LIB_NAME KF5MessageViewer
    DEPS "PimCommon MessageCore AkonadiCore AkonadiMime Contacts Libkleo MimeTreeParser" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageViewer
    )


install(FILES
    ${MessageViewer_Camelcasewebengine_HEADERS}
    ${MessageViewer_Camelcaseheader_HEADERS}
    ${MessageViewer_Camelcaseviewerplugin_HEADERS}
    ${MessageViewer_Camelcasesettings_HEADERS}
    ${MessageViewer_Camelcaseutils_HEADERS}
    ${MessageViewer_Camelcaseinterfaces_HEADERS}
    ${MessageViewer_Camelcaseviewer_HEADERS}
    ${MessageViewer_Camelcasewidgets_HEADERS}
    ${MessageViewer_Camelcaseantispam_HEADERS}
    ${MessageViewer_Camelfindbar_HEADERS}
    ${MessageViewer_Camelcasescam_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageViewer
    COMPONENT Devel
    )

install(FILES
    ${MessageViewer_webengine_HEADERS}
    ${MessageViewer_scam_HEADERS}
    ${MessageViewer_viewerplugin_HEADERS}
    ${MessageViewer_settings_HEADERS}
    ${MessageViewer_header_HEADERS}
    ${MessageViewer_utils_HEADERS}
    ${MessageViewer_interfaces_HEADERS}
    ${MessageViewer_HEADERS}
    ${MessageViewer_viewer_HEADERS}
    ${MessageViewer_widgets_HEADERS}
    ${MessageViewer_antispam_HEADERS}
    ${MessageViewer_findbar_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messageviewer_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/globalsettings_messageviewer.h
    ${CMAKE_CURRENT_BINARY_DIR}/messageviewer_debug.h
    ${CMAKE_CURRENT_BINARY_DIR}/config-messageviewer.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/messageviewer
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


install(FILES header/data/messageviewer_header_themes.knsrc DESTINATION ${KDE_INSTALL_CONFDIR} )

install(FILES notify/messageviewer.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR} )
install(FILES scamdetection/data/longurlServices.json DESTINATION ${KDE_INSTALL_DATADIR}/messageviewer )
