# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

option(KDEPIM_FOLDEROPEN_PROFILE "Enable debug output for timing folder loading" FALSE)
configure_file(config-messagelist.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-messagelist.h)
add_library(KPim${KF_MAJOR_VERSION}MessageList)
add_library(KPim${KF_MAJOR_VERSION}::MessageList ALIAS KPim${KF_MAJOR_VERSION}MessageList)

add_definitions(-DTRANSLATION_DOMAIN=\"libmessagelist\")

add_subdirectory(pics)
if(BUILD_TESTING)
	add_subdirectory(core/widgets/autotests)
endif()
target_sources(KPim${KF_MAJOR_VERSION}MessageList PRIVATE
    core/aggregation.cpp
    core/delegate.cpp
    core/filter.cpp
    core/item.cpp
    core/groupheaderitem.cpp
    core/manager.cpp
    core/messageitem.cpp
    core/messageitemsetmanager.cpp
    core/model.cpp
    core/modelinvariantindex.cpp
    core/modelinvariantrowmapper.cpp
    core/optionset.cpp
    core/theme.cpp
    core/themedelegate.cpp
    core/threadingcache.cpp
    core/storagemodelbase.cpp
    core/sortorder.cpp
    core/view.cpp
    core/widgetbase.cpp
    core/filtersavedmanager.cpp
    core/widgets/quicksearchline.cpp
    core/widgets/quicksearchwarning.cpp
    core/widgets/searchlinestatus.cpp
    core/widgets/searchcollectionindexingwarning.cpp
    core/widgets/tablockedwarning.cpp
    core/widgets/filtersavedmenu.cpp
    core/widgets/configurefiltersdialog.cpp
    core/widgets/configurefilterswidget.cpp
    core/widgets/filternamedialog.cpp
    core/widgets/filternamewidget.cpp

    utils/aggregationcombobox.cpp
    utils/aggregationconfigbutton.cpp
    utils/aggregationeditor.cpp
    utils/comboboxutils.cpp
    utils/configureaggregationsdialog.cpp
    utils/configurethemesdialog.cpp
    utils/optionseteditor.cpp
    utils/themecombobox.cpp
    utils/themeconfigbutton.cpp
    utils/themeeditor.cpp

    pane.cpp
    storagemodel.cpp
    widget.cpp
    messagelistutil.cpp
    utils/aggregationconfigbutton.h
    utils/themecombobox_p.h
    utils/themeconfigbutton.h
    utils/configureaggregationsdialog_p.h
    utils/aggregationcombobox_p.h
    utils/themecombobox.h
    utils/configureaggregationsdialog.h
    utils/optionseteditor.h
    utils/configurethemesdialog.h
    utils/aggregationeditor.h
    utils/aggregationcombobox.h
    utils/comboboxutils.h
    utils/configurethemesdialog_p.h
    utils/themeeditor.h
    messagelist_private_export.h
    pane.h
    storagemodel.h
    widget.h
    messagelistutil.h
    messagelistutil_p.h
    core/groupheaderitem.h
    core/optionset.h
    core/item_p.h
    core/filtersavedmanager.h
    core/manager.h
    core/model.h
    core/messageitem_p.h
    core/storagemodelbase.h
    core/aggregation.h
    core/enums.h
    core/messageitemsetmanager.h
    core/modelinvariantrowmapper_p.h
    core/sortorder.h
    core/themedelegate.h
    core/widgetbase.h
    core/modelinvariantindex.h
    core/model_p.h
    core/view.h
    core/filter.h
    core/threadingcache.h
    core/theme.h
    core/widgets/tablockedwarning.h
    core/widgets/filtersavedmenu.h
    core/widgets/quicksearchwarning.h
    core/widgets/filternamewidget.h
    core/widgets/filternamedialog.h
    core/widgets/configurefiltersdialog.h
    core/widgets/quicksearchline.h
    core/widgets/searchlinestatus.h
    core/widgets/searchcollectionindexingwarning.h
    core/widgets/configurefilterswidget.h
    core/modelinvariantrowmapper.h
    core/messageitem.h
    core/modelinvariantindex_p.h
    core/delegate.h
    core/item.h

    )
ecm_qt_declare_logging_category(KPim${KF_MAJOR_VERSION}MessageList HEADER messagelist_debug.h IDENTIFIER MESSAGELIST_LOG CATEGORY_NAME org.kde.pim.messagelist
        DESCRIPTION "messagelib (messagelist)"
        OLD_CATEGORY_NAMES log_messagelist
        EXPORT MESSAGELIB
    )

kconfig_add_kcfg_files(KPim${KF_MAJOR_VERSION}MessageList
    core/messagelistsettings.kcfgc
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim${KF_MAJOR_VERSION}MessageList PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KPim${KF_MAJOR_VERSION}MessageList BASE_NAME messagelist)

target_include_directories(KPim${KF_MAJOR_VERSION}MessageList INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MessageList/>")

target_link_libraries(KPim${KF_MAJOR_VERSION}MessageList
    PUBLIC
    KPim${KF_MAJOR_VERSION}::AkonadiCore
    KPim${KF_MAJOR_VERSION}::AkonadiMime
    KPim${KF_MAJOR_VERSION}::Mime
    PRIVATE
    KF${KF_MAJOR_VERSION}::KIOCore
    KPim${KF_MAJOR_VERSION}::MessageCore
    KF5::PimCommonAkonadi
    KPim${KF_MAJOR_VERSION}::AkonadiSearchPIM
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::IconThemes
    KF${KF_MAJOR_VERSION}::Completion
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::TextWidgets
    )

set_target_properties(KPim${KF_MAJOR_VERSION}MessageList PROPERTIES
    VERSION ${MESSAGELIST_VERSION}
    SOVERSION ${MESSAGELIST_SOVERSION}
    EXPORT_NAME MessageList
    )

install(TARGETS
    KPim${KF_MAJOR_VERSION}MessageList
    EXPORT KPim${KF_MAJOR_VERSION}MessageListTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    )

ecm_generate_headers(MessageList_Camelcasecore_HEADERS
    HEADER_NAMES
    View
    Enums
    StorageModelBase
    WidgetBase
    REQUIRED_HEADERS MessageList_core_HEADERS
    RELATIVE core
    PREFIX MessageList

    )

ecm_generate_headers(MessageList_Camelcaseutil_HEADERS
    HEADER_NAMES
    AggregationComboBox
    ThemeComboBox
    AggregationConfigButton
    ThemeConfigButton
    REQUIRED_HEADERS MessageList_util_HEADERS
    RELATIVE utils
    PREFIX MessageList

    )

ecm_generate_headers(MessageList_Camelcasewidgets_HEADERS
    HEADER_NAMES
    QuickSearchLine
    REQUIRED_HEADERS MessageList_widgets_HEADERS
    RELATIVE core/widgets
    PREFIX MessageList

    )


ecm_generate_headers(MessageList_CamelCase_HEADERS
    HEADER_NAMES
    Pane
    StorageModel
    MessageListUtil
    REQUIRED_HEADERS MessageList_HEADERS
    PREFIX MessageList
    )

ecm_generate_pri_file(BASE_NAME MessageList
    LIB_NAME KPim${KF_MAJOR_VERSION}MessageList
    DEPS "AkonadiCore AkonadiMime KMime" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MessageList
    )


install(FILES
    ${MessageList_Camelcasewidgets_HEADERS}
    ${MessageList_CamelCase_HEADERS}
    ${MessageList_Camelcasecore_HEADERS}
    ${MessageList_Camelcaseutil_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MessageList/MessageList
    COMPONENT Devel
    )

install(FILES
    ${MessageList_HEADERS}
    ${MessageList_widgets_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messagelist_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/messagelistsettings.h
    ${MessageList_core_HEADERS}
    ${MessageList_util_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/MessageList/messagelist
    COMPONENT Devel
    )
install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

if (BUILD_QCH)
    ecm_add_qch(
        KPim${KF_MAJOR_VERSION}MessageList_QCH
        NAME KPim${KF_MAJOR_VERSION}MessageList
        BASE_NAME KPim${KF_MAJOR_VERSION}MessageList
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${MessageList_HEADERS}
        ${MessageList_widgets_HEADERS}
        ${MessageList_core_HEADERS}
        ${MessageList_util_HEADERS}
        LINK_QCHS
            Qt${QT_MAJOR_VERSION}Core_QCH
            Qt${QT_MAJOR_VERSION}Gui_QCH
            Qt${QT_MAJOR_VERSION}Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            MESSAGELIST_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )

    ecm_install_qch_export(
        TARGETS KPim${KF_MAJOR_VERSION}MessageList_QCH
        FILE KPim${KF_MAJOR_VERSION}MessageListQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim${KF_MAJOR_VERSION}MessageListQchTargets.cmake\")")
endif()
