# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

add_definitions(-DTRANSLATION_DOMAIN=\"libwebengineviewer\")
include_directories(${CMAKE_BINARY_DIR}/webengineviewer/src ${CMAKE_BINARY_DIR}/webengineviewer)

add_library(KPim${KF_MAJOR_VERSION}WebEngineViewer)
add_library(KPim${KF_MAJOR_VERSION}::WebEngineViewer ALIAS KPim${KF_MAJOR_VERSION}WebEngineViewer)


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



target_sources(KPim${KF_MAJOR_VERSION}WebEngineViewer PRIVATE
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

    findbar/findbarwebengineview.h
    findbar/findbarbase.h
    webenginenavigationrequestinterceptor.h
    webhittestresult.h
    urlinterceptor/networkurlinterceptor.h
    urlinterceptor/networkpluginurlinterceptorconfigurewidget.h
    urlinterceptor/loadexternalreferencesurlinterceptor/loadexternalreferencesurlinterceptor.h
    urlinterceptor/blockexternalresourcesurlinterceptor/blockexternalresourcesurlinterceptor.h
    urlinterceptor/networkpluginurlinterceptor.h
    urlinterceptor/blocktrackingurlinterceptor/blocktrackingurlinterceptor.h
    urlinterceptor/networkurlinterceptorpluginmanager.h
    urlinterceptor/networkpluginurlinterceptorinterface.h
    urlinterceptor/networkurlinterceptormanager.h
    webenginepage.h
    webhittest.h
    webenginescript.h
    webengineexporthtmlpagejob.h
    webengineviewer_private_export.h
    webenginemanagescript.h
    webengineexportpdfpagejob.h
    webengineview.h
    widgets/tracking/trackingdetailsdialog.h
    widgets/tracking/trackingwarningwidget.h
    widgets/zoomactionmenu.h
    widgets/submittedformwidgets/submittedformwarningwidget.h
    checkphishingurl/localdatabasemanager_p.h
    checkphishingurl/localdatabasemanager.h
    checkphishingurl/urlhashing.h
    checkphishingurl/searchfullhashjob.h
    checkphishingurl/createphishingurldatabasejob.h
    checkphishingurl/updatedatabaseinfo.h
    checkphishingurl/backoffmodemanager.h
    checkphishingurl/localdatabasefile.h
    checkphishingurl/riceencodingdecoder.h
    checkphishingurl/hashcachemanager.h
    checkphishingurl/downloadlocaldatabasethread.h
    checkphishingurl/checkphishingurljob.h
    checkphishingurl/checkphishingurlutil.h
    checkphishingurl/createdatabasefilejob.h
    checkphishingurl/checkphishingurlcache.h
    developertool/developertooldialog.h
    developertool/developertoolwidget.h
    networkmanager/interceptormanager.h
    webengineaccesskey/webengineaccesskeyutils.h
    webengineaccesskey/webengineaccesskey.h
    webengineaccesskey/webengineaccesskeyanchor.h
    )

ecm_qt_declare_logging_category(KPim${KF_MAJOR_VERSION}WebEngineViewer HEADER webengineviewer_debug.h IDENTIFIER WEBENGINEVIEWER_LOG CATEGORY_NAME org.kde.pim.webengineviewer
        DESCRIPTION "messagelib (webengineviewer)"
        EXPORT MESSAGELIB
    )
ecm_qt_declare_logging_category(KPim${KF_MAJOR_VERSION}WebEngineViewer HEADER webengineviewer_block_tracking_url_interceptor_debug.h
    IDENTIFIER WEBENGINEVIEWER_BLOCK_TRACKING_URL_LOG CATEGORY_NAME org.kde.pim.webengineviewer_block_tracking_url
        DESCRIPTION "messagelib (webengineviewer block tracking url)"
        EXPORT MESSAGELIB
    )


if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim${KF_MAJOR_VERSION}WebEngineViewer PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KPim${KF_MAJOR_VERSION}WebEngineViewer BASE_NAME webengineviewer)

target_include_directories(KPim${KF_MAJOR_VERSION}WebEngineViewer INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/WebEngineViewer/>")

target_link_libraries(KPim${KF_MAJOR_VERSION}WebEngineViewer
    PUBLIC
    Qt::WebEngineWidgets
    KPim${KF_MAJOR_VERSION}::PimCommon
    KPim${KF_MAJOR_VERSION}::PimTextEdit
    PRIVATE
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    KF${KF_MAJOR_VERSION}::ConfigCore
    )

if (WIN32)
    target_link_libraries(KPim${KF_MAJOR_VERSION}WebEngineViewer PRIVATE Ws2_32)
endif()

set_target_properties(KPim${KF_MAJOR_VERSION}WebEngineViewer PROPERTIES
    VERSION ${WEBENGINEVIEWER_VERSION}
    SOVERSION ${WEBENGINEVIEWER_SOVERSION}
    EXPORT_NAME WebEngineViewer
    )

install(TARGETS
    KPim${KF_MAJOR_VERSION}WebEngineViewer
    EXPORT KPim${KF_MAJOR_VERSION}WebEngineViewerTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
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
    LIB_NAME KPim${KF_MAJOR_VERSION}WebEngineViewer
    DEPS "webenginewidgets PimCommon" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/WebEngineViewer
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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/WebEngineViewer/WebEngineViewer
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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/WebEngineViewer/webengineviewer
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if (BUILD_QCH)
    ecm_add_qch(
        KPim${KF_MAJOR_VERSION}WebEngineViewer_QCH
        NAME KPim${KF_MAJOR_VERSION}WebEngineViewer
        BASE_NAME KPim${KF_MAJOR_VERSION}WebEngineViewer
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
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
        LINK_QCHS
            Qt${QT_MAJOR_VERSION}Core_QCH
            Qt${QT_MAJOR_VERSION}Gui_QCH
            Qt${QT_MAJOR_VERSION}Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            WEBENGINEVIEWER_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )

    ecm_install_qch_export(
        TARGETS KPim${KF_MAJOR_VERSION}WebEngineViewer_QCH
        FILE KPim${KF_MAJOR_VERSION}WebEngineViewerQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim${KF_MAJOR_VERSION}WebEngineViewerQchTargets.cmake\")")
endif()
