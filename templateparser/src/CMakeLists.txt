# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

add_definitions(-DTRANSLATION_DOMAIN=\"libtemplateparser\")
add_library(KF5TemplateParser)
add_library(KF5::TemplateParser ALIAS KF5TemplateParser)


target_sources(KF5TemplateParser PRIVATE
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
ecm_qt_declare_logging_category(KF5TemplateParser HEADER templateparser_debug.h IDENTIFIER TEMPLATEPARSER_LOG CATEGORY_NAME org.kde.pim.templateparser
        DESCRIPTION "messagelib (templateparser)"
        OLD_CATEGORY_NAMES log_templateparser
        EXPORT MESSAGELIB
    )



kconfig_add_kcfg_files(KF5TemplateParser settings/templatesconfiguration_kfg.kcfgc)

kconfig_add_kcfg_files(KF5TemplateParser
    settings/customtemplates_kfg.kcfgc
    settings/globalsettings_templateparser.kcfgc
    )

ki18n_wrap_ui(KF5TemplateParser
    ui/templatesconfiguration_base.ui
    ui/customtemplates_base.ui
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5TemplateParser PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KF5TemplateParser BASE_NAME templateparser)

target_link_libraries(KF5TemplateParser
    PRIVATE
    KF5::Mime
    KF5::MimeTreeParser
    KF5::IdentityManagement
    KF5::MessageCore
    KF5::XmlGui
    KF5::Completion
    KF5::CoreAddons
    KF5::I18n
    KF5::SonnetUi
    KF5::SyntaxHighlighting
    Qt${QT_MAJOR_VERSION}::WebEngineWidgets
    KF5::Libkdepim
    KF5::PimTextEdit
    )
set_target_properties(KF5TemplateParser PROPERTIES
    VERSION ${TEMPLATEPARSER_VERSION}
    SOVERSION ${TEMPLATEPARSER_SOVERSION}
    EXPORT_NAME TemplateParser
    )

target_include_directories(KF5TemplateParser INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/TemplateParser/>")

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

ecm_generate_pri_file(BASE_NAME TemplateParser
    LIB_NAME KF5TemplateParser
    DEPS "" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/TemplateParser
    )


install(TARGETS KF5TemplateParser EXPORT KF5TemplateParserTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})



install(FILES
    ${TemplateParser_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/TemplateParser/TemplateParser
    COMPONENT Devel
    )

install(FILES
    ${TemplateParser_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/templateparser_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/globalsettings_templateparser.h
    ${CMAKE_CURRENT_BINARY_DIR}/customtemplates_kfg.h
    ${CMAKE_CURRENT_BINARY_DIR}/ui_templatesconfiguration_base.h
    ${CMAKE_CURRENT_BINARY_DIR}/templatesconfiguration_kfg.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/TemplateParser/templateparser
    COMPONENT Devel
    )

install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

install(FILES syntax/kmail-template.xml DESTINATION ${KDE_INSTALL_DATADIR}/org.kde.syntax-highlighting/syntax)

if (BUILD_QCH)
    ecm_add_qch(
        KF5TemplateParser_QCH
        NAME KF5TemplateParser
        BASE_NAME KF5TemplateParser
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${TemplateParser_HEADERS}
        LINK_QCHS
            Qt5Core_QCH
            Qt5Gui_QCH
            Qt5Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            TEMPLATEPARSER_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )

    ecm_install_qch_export(
        TARGETS KF5TemplateParser_QCH
        FILE KF5TemplateParserQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5TemplateParserQchTargets.cmake\")")
endif()
