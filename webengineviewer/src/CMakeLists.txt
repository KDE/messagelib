add_definitions(-DTRANSLATION_DOMAIN=\"libwebengineviewer\")
include_directories(${WebEngineViewer_BINARY_DIR}/src)

if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
    add_subdirectory(webengineaccesskey/autotests)
    add_subdirectory(findbar/autotests)
    add_subdirectory(checkphishingurl/autotests/)
    add_subdirectory(checkphishingurl/tests/)
endif()

if (Qt5WebEngineWidgets_VERSION VERSION_GREATER "5.7.99")
    set(WEBENGINEVIEWER_PRINT_SUPPORT true)
endif()


configure_file(../config-webengineviewer.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/../config-webengineviewer.h )

set(libwebengineviewer_webengine_SRCS
    networkmanager/interceptormanager.cpp
    webhittestresult.cpp
    webhittest.cpp
    webenginepage.cpp
    webenginescript.cpp
    webengineview.cpp
    webenginemanagescript.cpp
    webengineprintmessagebox.cpp
    webengineexporthtmlpagejob.cpp
    webenginenavigationrequestinterceptor.cpp
    )

set(libwebengineviewer_checkphishingurl_SRCS
   checkphishingurl/checkphishingurljob.cpp
   checkphishingurl/checkphishingurlcache.cpp
   checkphishingurl/createphishingurldatabasejob.cpp
   checkphishingurl/checkphishingurlfromlocaldatabasejob.cpp
   checkphishingurl/localdatabasemanager.cpp
   checkphishingurl/checkphishingurlutil.cpp
   checkphishingurl/searchfullhashjob.cpp
   checkphishingurl/localdatabasefile.cpp
   )

set(libwebengineviewer_interceptor_SRCS
    urlinterceptor/networkurlinterceptor.cpp
    urlinterceptor/networkpluginurlinterceptorinterface.cpp
    urlinterceptor/networkurlinterceptorpluginmanager.cpp
    urlinterceptor/networkpluginurlinterceptor.cpp
    urlinterceptor/networkurlinterceptormanager.cpp
    urlinterceptor/networkpluginurlinterceptorconfigurewidget.cpp
    )

set(libwebengineviewer_accesskey_SRCS
    webengineaccesskey/webengineaccesskey.cpp
    webengineaccesskey/webengineaccesskeyanchor.cpp
    webengineaccesskey/webengineaccesskeyutils.cpp
    )

set(libwebengineviewer_findbar_SRCS
    findbar/findbarbase.cpp
    findbar/findbarwebengineview.cpp
    )

set(libwebengineviewer_widgets_SRCS
    widgets/zoomactionmenu.cpp
    )

set(libwebengineviewer_SRCS
    ${libwebengineviewer_checkphishingurl_SRCS}
    ${libwebengineviewer_interceptor_SRCS}
    ${libwebengineviewer_webengine_SRCS}
    ${libwebengineviewer_findbar_SRCS}
    ${libwebengineviewer_widgets_SRCS}
    ${libwebengineviewer_print_SRCS}
    ${libwebengineviewer_accesskey_SRCS}
    )

qt5_add_resources(libwebengineviewer_webengine_SRCS jquery.qrc)

ecm_qt_declare_logging_category(libwebengineviewer_webengine_SRCS HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer)


add_library(KF5WebEngineViewer ${libwebengineviewer_SRCS} ${libwebengineviewer_webengine_SRCS})
generate_export_header(KF5WebEngineViewer BASE_NAME webengineviewer)
add_library(KF5::WebEngineViewer ALIAS KF5WebEngineViewer)

target_include_directories(KF5WebEngineViewer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/WebEngineViewer/;${KDE_INSTALL_INCLUDEDIR_KF5}/webengineviewer>")
target_include_directories(KF5WebEngineViewer PUBLIC "$<BUILD_INTERFACE:${WebEngineViewer_SOURCE_DIR}/src;${WebEngineViewer_BINARY_DIR}/src;>")

target_link_libraries(KF5WebEngineViewer
    PUBLIC
    Qt5::WebEngineWidgets
    KF5::PimCommon
    PRIVATE
    KF5::CoreAddons
    KF5::XmlGui
    KF5::Completion
    KF5::I18n
    KF5::WidgetsAddons
    KF5::ConfigCore
    Qt5::Sql
    )

set_target_properties(KF5WebEngineViewer PROPERTIES
    VERSION ${WEBENGINEVIEWER_VERSION_STRING}
    SOVERSION ${WEBENGINEVIEWER_SOVERSION}
    EXPORT_NAME WebEngineViewer
    )

install(TARGETS
    KF5WebEngineViewer
    EXPORT KF5WebEngineViewerTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK}
    )

ecm_generate_headers(WebEngineViewer_Camelfindbar_HEADERS
    HEADER_NAMES
    FindBarBase
    FindBarWebEngineView
    REQUIRED_HEADERS WebEngineViewer_findbar_HEADERS
    PREFIX WebEngineViewer
    RELATIVE findbar
    )

ecm_generate_headers(WebEngineViewer_Camelcasewebengine_accesskey_HEADERS
    HEADER_NAMES
    WebEngineAccessKey

    REQUIRED_HEADERS WebEngineViewer_webengine_accesskey_HEADERS
    PREFIX WebEngineViewer
    RELATIVE webengineaccesskey
    )
ecm_generate_headers(WebEngineViewer_Camelcasewebengine_urlinterceptor_HEADERS
    HEADER_NAMES
    NetworkUrlInterceptorPluginManager
    NetworkUrlInterceptor
    NetworkPluginUrlInterceptorInterface
    NetworkPluginUrlInterceptor
    NetworkPluginUrlInterceptorConfigureWidget

    REQUIRED_HEADERS WebEngineViewer_webengine_urlinterceptor_HEADERS
    PREFIX WebEngineViewer
    RELATIVE urlinterceptor
    )
ecm_generate_headers(WebEngineViewer_Camelcasewebengine_manager_HEADERS
    HEADER_NAMES
    InterceptorManager
    REQUIRED_HEADERS WebEngineViewer_webengine_manager_HEADERS
    PREFIX WebEngineViewer
    RELATIVE networkmanager
    )
ecm_generate_headers(WebEngineViewer_Camelcasewebengine_checkurl_HEADERS
    HEADER_NAMES
    CheckPhishingUrlJob
    CheckPhishingUrlCache
    CreatePhishingUrlDataBaseJob
    CheckPhishingUrlFromLocalDataBaseJob
    LocalDataBaseManager
    SearchFullHashJob
    REQUIRED_HEADERS WebEngineViewer_webengine_checkurl_HEADERS
    PREFIX WebEngineViewer
    RELATIVE checkphishingurl
    )
ecm_generate_headers(WebEngineViewer_Camelcasewebengine_misc_HEADERS
    HEADER_NAMES
    WebHitTestResult
    WebEnginePage
    WebEngineView
    WebHitTest
    WebEngineScript
    WebEngineManageScript
    WebEnginePrintMessageBox
    WebEngineExportHtmlPageJob
    REQUIRED_HEADERS WebEngineViewer_webengine_misc_HEADERS
    PREFIX WebEngineViewer
    RELATIVE
    )

ecm_generate_headers(WebEngineViewer_Camelcasewidgets_HEADERS
    HEADER_NAMES
    ZoomActionMenu
    REQUIRED_HEADERS WebEngineViewer_widgets_HEADERS
    PREFIX WebEngineViewer
    RELATIVE widgets
    )



ecm_generate_pri_file(BASE_NAME WebEngineViewer
    LIB_NAME KF5WebEngineViewer
    DEPS "" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/WebEngineViewer
    )


install(FILES
    ${WebEngineViewer_Camelcasewebengine_checkurl_HEADERS}
    ${WebEngineViewer_Camelcasewebengine_accesskey_HEADERS}
    ${WebEngineViewer_Camelcasewebengine_urlinterceptor_HEADERS}
    ${WebEngineViewer_Camelcasewebengine_manager_HEADERS}
    ${WebEngineViewer_Camelcasewebengine_misc_HEADERS}
    ${WebEngineViewer_Camelfindbar_HEADERS}
    ${WebEngineViewer_Camelcasewidgets_HEADERS}
    ${WebEngineViewer_Camelprint_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/WebEngineViewer
    COMPONENT Devel
    )

install(FILES
    ${WebEngineViewer_webengine_checkurl_HEADERS}
    ${WebEngineViewer_webengine_accesskey_HEADERS}
    ${WebEngineViewer_findbar_HEADERS}
    ${WebEngineViewer_webengine_urlinterceptor_HEADERS}
    ${WebEngineViewer_webengine_manager_HEADERS}
    ${WebEngineViewer_webengine_misc_HEADERS}
    ${WebEngineViewer_widgets_HEADERS}
    ${WebEngineViewer_print_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/../config-webengineviewer.h
    ${CMAKE_CURRENT_BINARY_DIR}/webengineviewer_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/webengineviewer
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

