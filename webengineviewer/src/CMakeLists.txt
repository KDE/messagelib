add_definitions(-DTRANSLATION_DOMAIN=\"libwebengineviewer\")
include_directories(${CMAKE_BINARY_DIR}/webengineviewer/src ${CMAKE_BINARY_DIR}/webengineviewer)

add_library(KF5WebEngineViewer)
add_library(KF5::WebEngineViewer ALIAS KF5WebEngineViewer)


if(BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
    add_subdirectory(webengineaccesskey/autotests)
    add_subdirectory(findbar/autotests)
    add_subdirectory(checkphishingurl/autotests/)
    add_subdirectory(checkphishingurl/tests/)
    add_subdirectory(urlinterceptor/blockexternalresourcesurlinterceptor/autotests)
    add_subdirectory(widgets/tracking/autotests/)
    add_subdirectory(developertool/autotests)
endif()



target_sources(KF5WebEngineViewer PRIVATE
    networkmanager/interceptormanager.cpp
    webhittestresult.cpp
    webhittest.cpp
    webenginepage.cpp
    webenginescript.cpp
    webengineview.cpp
    webenginemanagescript.cpp
    webengineexporthtmlpagejob.cpp
    webenginenavigationrequestinterceptor.cpp
    webengineexportpdfpagejob.cpp

    checkphishingurl/checkphishingurljob.cpp
    checkphishingurl/checkphishingurlcache.cpp
    checkphishingurl/createphishingurldatabasejob.cpp
    checkphishingurl/localdatabasemanager.cpp
    checkphishingurl/checkphishingurlutil.cpp
    checkphishingurl/searchfullhashjob.cpp
    checkphishingurl/localdatabasefile.cpp
    checkphishingurl/createdatabasefilejob.cpp
    checkphishingurl/updatedatabaseinfo.cpp
    checkphishingurl/riceencodingdecoder.cpp
    checkphishingurl/urlhashing.cpp
    checkphishingurl/hashcachemanager.cpp
    checkphishingurl/backoffmodemanager.cpp
    checkphishingurl/downloadlocaldatabasethread.cpp

    urlinterceptor/networkurlinterceptor.cpp
    urlinterceptor/networkpluginurlinterceptorinterface.cpp
    urlinterceptor/networkurlinterceptorpluginmanager.cpp
    urlinterceptor/networkpluginurlinterceptor.cpp
    urlinterceptor/networkurlinterceptormanager.cpp
    urlinterceptor/networkpluginurlinterceptorconfigurewidget.cpp
    urlinterceptor/blocktrackingurlinterceptor/blocktrackingurlinterceptor.cpp
    urlinterceptor/loadexternalreferencesurlinterceptor/loadexternalreferencesurlinterceptor.cpp
    urlinterceptor/blockexternalresourcesurlinterceptor/blockexternalresourcesurlinterceptor.cpp

    webengineaccesskey/webengineaccesskey.cpp
    webengineaccesskey/webengineaccesskeyanchor.cpp
    webengineaccesskey/webengineaccesskeyutils.cpp

    findbar/findbarbase.cpp
    findbar/findbarwebengineview.cpp

    widgets/zoomactionmenu.cpp

    widgets/tracking/trackingdetailsdialog.cpp
    widgets/tracking/trackingwarningwidget.cpp

    developertool/developertoolwidget.cpp
    developertool/developertooldialog.cpp
    widgets/submittedformwidgets/submittedformwarningwidget.cpp
    )

ecm_qt_declare_logging_category(KF5WebEngineViewer HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer
        DESCRIPTION "messagelib (webengineviewer)"
        EXPORT MESSAGELIB
    )


if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5WebEngineViewer PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KF5WebEngineViewer BASE_NAME webengineviewer)

target_include_directories(KF5WebEngineViewer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/WebEngineViewer/;${KDE_INSTALL_INCLUDEDIR_KF5}/webengineviewer>")

target_link_libraries(KF5WebEngineViewer
    PUBLIC
    Qt::WebEngineWidgets
    KF5::PimCommon
    KF5::PimTextEdit
    PRIVATE
    KF5::CoreAddons
    KF5::XmlGui
    KF5::I18n
    KF5::WidgetsAddons
    KF5::ConfigCore
    )

if (WIN32)
    target_link_libraries(KF5WebEngineViewer PRIVATE Ws2_32)
endif()

set_target_properties(KF5WebEngineViewer PROPERTIES
    VERSION ${WEBENGINEVIEWER_VERSION}
    SOVERSION ${WEBENGINEVIEWER_SOVERSION}
    EXPORT_NAME WebEngineViewer
    )

install(TARGETS
    KF5WebEngineViewer
    EXPORT KF5WebEngineViewerTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
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
    LocalDataBaseManager
    CheckPhishingUrlUtil
    SearchFullHashJob
    UpdateDataBaseInfo
    HashCacheManager
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
    WebEngineExportHtmlPageJob
    WebEngineExportPdfPageJob
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

ecm_generate_headers(WebEngineViewer_Camelblocktrackingurlinterceptor_HEADERS
    HEADER_NAMES
    BlockTrackingUrlInterceptor

    REQUIRED_HEADERS WebEngineViewer_blocktrackingurlinterceptor_HEADERS
    PREFIX WebEngineViewer
    RELATIVE urlinterceptor/blocktrackingurlinterceptor/
    )

ecm_generate_headers(WebEngineViewer_Camelloadexternalreferencesurlinterceptor_HEADERS
    HEADER_NAMES
    LoadExternalReferencesUrlInterceptor

    REQUIRED_HEADERS WebEngineViewer_loadexternalreferencesurlinterceptor_HEADERS
    PREFIX WebEngineViewer
    RELATIVE urlinterceptor/loadexternalreferencesurlinterceptor/
    )

ecm_generate_headers(WebEngineViewer_Camelblockexternalresourcesurlinterceptor_HEADERS
    HEADER_NAMES
    BlockExternalResourcesUrlInterceptor

    REQUIRED_HEADERS WebEngineViewer_blockexternalresourcesurlinterceptor_HEADERS
    PREFIX WebEngineViewer
    RELATIVE urlinterceptor/blockexternalresourcesurlinterceptor/
    )


ecm_generate_headers(WebEngineViewer_Cameltracking_HEADERS
    HEADER_NAMES
    TrackingWarningWidget

    REQUIRED_HEADERS WebEngineViewer_tracking_HEADERS
    PREFIX WebEngineViewer
    RELATIVE widgets/tracking/
    )

ecm_generate_headers(WebEngineViewer_Cameldevelopertool_HEADERS
    HEADER_NAMES
    DeveloperToolDialog

    REQUIRED_HEADERS WebEngineViewer_developertool_HEADERS
    PREFIX WebEngineViewer
    RELATIVE developertool/
    )

ecm_generate_headers(WebEngineViewer_Camelsubmittedformwidget_HEADERS
    HEADER_NAMES
    SubmittedFormWarningWidget 

    REQUIRED_HEADERS WebEngineViewer_submittedformwidget_HEADERS
    PREFIX WebEngineViewer
    RELATIVE widgets/submittedformwidgets/
    )


ecm_generate_pri_file(BASE_NAME WebEngineViewer
    LIB_NAME KF5WebEngineViewer
    DEPS "webenginewidgets PimCommon" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/WebEngineViewer
    )


install(FILES
    ${WebEngineViewer_Camelsubmittedformwidget_HEADERS}
    ${WebEngineViewer_Cameldevelopertool_HEADERS}
    ${WebEngineViewer_Cameltracking_HEADERS}
    ${WebEngineViewer_Camelblockexternalresourcesurlinterceptor_HEADERS}
    ${WebEngineViewer_Camelloadexternalreferencesurlinterceptor_HEADERS}
    ${WebEngineViewer_Camelblocktrackingurlinterceptor_HEADERS}
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
    ${WebEngineViewer_submittedformwidget_HEADERS}
    ${WebEngineViewer_developertool_HEADERS}
    ${WebEngineViewer_tracking_HEADERS}
    ${WebEngineViewer_blockexternalresourcesurlinterceptor_HEADERS}
    ${WebEngineViewer_loadexternalreferencesurlinterceptor_HEADERS}
    ${WebEngineViewer_blocktrackingurlinterceptor_HEADERS}
    ${WebEngineViewer_webengine_checkurl_HEADERS}
    ${WebEngineViewer_webengine_accesskey_HEADERS}
    ${WebEngineViewer_findbar_HEADERS}
    ${WebEngineViewer_webengine_urlinterceptor_HEADERS}
    ${WebEngineViewer_webengine_manager_HEADERS}
    ${WebEngineViewer_webengine_misc_HEADERS}
    ${WebEngineViewer_widgets_HEADERS}
    ${WebEngineViewer_print_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/webengineviewer_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/webengineviewer
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

