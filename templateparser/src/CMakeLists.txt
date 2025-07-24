# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

ecm_setup_version(PROJECT VARIABLE_PREFIX TEMPLATEPARSER
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/templateparser_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6TemplateParserConfigVersion.cmake"
    SOVERSION 6
)

add_definitions(-DTRANSLATION_DOMAIN=\"libtemplateparser6\")
add_library(KPim6TemplateParser)
add_library(KPim6::TemplateParser ALIAS KPim6TemplateParser)


target_sources(KPim6TemplateParser PRIVATE
    templateparserjob.cpp
    defaulttemplates.cpp
    templatesutil.cpp
    customtemplates.cpp
    customtemplatesmenu.cpp
    templatesconfiguration.cpp
    templatesinsertcommandpushbutton.cpp
    templatescommandmenu.cpp
    templatesinsertcommandaction.cpp
    templatestextedit.cpp
    templatestexteditor.cpp
    templateextracttextfrommail.cpp
    templatewebenginepage.cpp
    templateextracthtmlelementfrommail.cpp
    templateparserextracthtmlinfo.cpp
    templateparserextracthtmlinforesult.cpp
    templateparseremailaddressrequesterbase.cpp
    templateparseremailaddressrequesterlineedit.cpp
    templateparseremailaddressrequesterinterfacewidget.cpp
    templateconvertcommandjob.cpp

    templateextracttextfrommail.h
    templateparserjob.h
    templatestexteditor.h
    templateparseremailaddressrequesterbase.h
    templateparserextracthtmlinforesult.h
    templatescommandmenu.h
    templateparserjob_p.h
    templatestextedit.h
    templateparser_private_export.h
    templatesutil_p.h
    templatesconfiguration.h
    customtemplates.h
    templatewebenginepage.h
    templatesutil.h
    templateparseremailaddressrequesterinterfacewidget.h
    templateparserextracthtmlinfo.h
    defaulttemplates.h
    templatesinsertcommandpushbutton.h
    customtemplatesmenu.h
    templateconvertcommandjob.h
    templatesinsertcommandaction.h
    templateextracthtmlelementfrommail.h
    templateparseremailaddressrequesterlineedit.h
)
ecm_qt_declare_logging_category(KPim6TemplateParser HEADER templateparser_debug.h IDENTIFIER TEMPLATEPARSER_LOG CATEGORY_NAME org.kde.pim.templateparser
    DESCRIPTION "messagelib (templateparser)"
    OLD_CATEGORY_NAMES log_templateparser
    EXPORT MESSAGELIB
)



kconfig_add_kcfg_files(KPim6TemplateParser settings/templatesconfiguration_kfg.kcfgc)

kconfig_add_kcfg_files(KPim6TemplateParser
    settings/customtemplates_kfg.kcfgc
    settings/globalsettings_templateparser.kcfgc
)

ki18n_wrap_ui(KPim6TemplateParser
    ui/templatesconfiguration_base.ui
    ui/customtemplates_base.ui
)

if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim6TemplateParser PROPERTIES UNITY_BUILD ON)
endif()

ecm_generate_export_header(KPim6TemplateParser
    BASE_NAME templateparser
    VERSION ${TEMPLATEPARSER_VERSION}
    DEPRECATED_BASE_VERSION 0
    USE_VERSION_HEADER
)

target_link_libraries(KPim6TemplateParser
    PRIVATE
    KPim6::Mime
    KPim6::MimeTreeParser
    KPim6::IdentityManagementCore
    KPim6::MessageCore
    KF6::XmlGui
    KF6::Completion
    KF6::CoreAddons
    KF6::I18n
    KF6::Codecs
    KF6::SonnetUi
    KF6::SyntaxHighlighting
    Qt::WebEngineWidgets
    KPim6::PimTextEdit
    Gpgmepp
)

set_target_properties(KPim6TemplateParser PROPERTIES
    VERSION ${TEMPLATEPARSER_VERSION}
    SOVERSION ${TEMPLATEPARSER_SOVERSION}
    EXPORT_NAME TemplateParser
)

target_include_directories(KPim6TemplateParser INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim6/TemplateParser/>")

install(FILES
    settings/customtemplates_kfg.kcfg
    settings/templatesconfiguration_kfg.kcfg
    DESTINATION ${KDE_INSTALL_KCFGDIR}
)

ecm_generate_headers(TemplateParser_CamelCase_HEADERS
    HEADER_NAMES
    CustomTemplates
    CustomTemplatesMenu
    DefaultTemplates
    TemplatesConfiguration
    TemplatesTextEdit
    TemplatesUtil
    TemplatesInsertCommandPushButton
    TemplatesCommandMenu
    TemplatesInsertCommandAction
    TemplateParserJob
    TemplateParserExtractHtmlInfoResult
    TemplateParserEmailAddressRequesterBase
    TemplateConvertCommandJob
    REQUIRED_HEADERS TemplateParser_HEADERS
    PREFIX TemplateParser
)

install(TARGETS KPim6TemplateParser EXPORT KPim6TemplateParserTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})



install(FILES
    ${TemplateParser_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/TemplateParser/TemplateParser
    COMPONENT Devel
)

install(FILES
    ${TemplateParser_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/templateparser_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/globalsettings_templateparser.h
    ${CMAKE_CURRENT_BINARY_DIR}/customtemplates_kfg.h
    ${CMAKE_CURRENT_BINARY_DIR}/ui_templatesconfiguration_base.h
    ${CMAKE_CURRENT_BINARY_DIR}/templatesconfiguration_kfg.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/TemplateParser/templateparser
    COMPONENT Devel
)

install(FILES syntax/kmail-template.xml DESTINATION ${KDE_INSTALL_DATADIR}/org.kde.syntax-highlighting/syntax)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6TemplateParser")


if(BUILD_QCH)
    ecm_add_qch(
        KPim6TemplateParser_QCH
        NAME KPim6TemplateParser
        BASE_NAME KPim6TemplateParser
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${TemplateParser_HEADERS}
        LINK_QCHS
        Qt6Core_QCH
        Qt6Gui_QCH
        Qt6Widgets_QCH
        INCLUDE_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
        TEMPLATEPARSER_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
)

ecm_install_qch_export(
    TARGETS KPim6TemplateParser_QCH
    FILE KPim6TemplateParserQchTargets.cmake
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)
set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim6TemplateParserQchTargets.cmake\")")
endif()


configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimTemplateParserConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6TemplateParserConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6TemplateParserConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6TemplateParserConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT KPim6TemplateParserTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6TemplateParserTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/templateparser_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/TemplateParser COMPONENT Devel
)
