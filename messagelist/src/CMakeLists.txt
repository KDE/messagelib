# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

ecm_setup_version(PROJECT VARIABLE_PREFIX MESSAGELIST
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/messagelist_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageListConfigVersion.cmake"
    SOVERSION 6
)

option(KDEPIM_FOLDEROPEN_PROFILE "Enable debug output for timing folder loading" FALSE)
configure_file(config-messagelist.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-messagelist.h)
add_library(KPim6MessageList)
add_library(KPim6::MessageList ALIAS KPim6MessageList)

add_definitions(-DTRANSLATION_DOMAIN=\"libmessagelist6\")

add_subdirectory(pics)
if(BUILD_TESTING)
    add_subdirectory(core/widgets/autotests)
endif()
target_sources(KPim6MessageList PRIVATE
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

    core/widgets/searchstatusbuttons.h
    core/widgets/searchstatusbuttons.cpp

    core/widgets/searchmessagebybuttons.h
    core/widgets/searchmessagebybuttons.cpp

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
ecm_qt_declare_logging_category(KPim6MessageList HEADER messagelist_debug.h IDENTIFIER MESSAGELIST_LOG CATEGORY_NAME org.kde.pim.messagelist
    DESCRIPTION "messagelib (messagelist)"
    OLD_CATEGORY_NAMES log_messagelist
    EXPORT MESSAGELIB
)

kconfig_add_kcfg_files(KPim6MessageList
    core/messagelistsettings.kcfgc
)

if(COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim6MessageList PROPERTIES UNITY_BUILD ON)
endif()

ecm_generate_export_header(KPim6MessageList
    BASE_NAME messagelist
    VERSION ${MESSAGELIST_VERSION}
    DEPRECATED_BASE_VERSION 0
    USE_VERSION_HEADER
)


target_include_directories(KPim6MessageList INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageList/>")

target_link_libraries(KPim6MessageList
    PUBLIC
    KPim6::AkonadiCore
    KPim6::AkonadiMime
    KPim6::Mime
    PRIVATE
    KPim6::MessageCore
    KPim6::PimCommonAkonadi
    KPim6::AkonadiSearchPIM
    KF6::XmlGui
    KF6::IconThemes
    KF6::IconWidgets
    KF6::Completion
    KF6::I18n
    KF6::TextWidgets
    KF6::TextUtils
)

set_target_properties(KPim6MessageList PROPERTIES
    VERSION ${MESSAGELIST_VERSION}
    SOVERSION ${MESSAGELIST_SOVERSION}
    EXPORT_NAME MessageList
)

install(TARGETS
    KPim6MessageList
    EXPORT KPim6MessageListTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
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
    SearchMessageByButtons
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


install(FILES
    ${MessageList_Camelcasewidgets_HEADERS}
    ${MessageList_CamelCase_HEADERS}
    ${MessageList_Camelcasecore_HEADERS}
    ${MessageList_Camelcaseutil_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageList/MessageList
    COMPONENT Devel
)

install(FILES
    ${MessageList_HEADERS}
    ${MessageList_widgets_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messagelist_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/messagelistsettings.h
    ${MessageList_core_HEADERS}
    ${MessageList_util_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageList/messagelist
    COMPONENT Devel
)
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6MessageList")


if(BUILD_QCH)
    ecm_add_qch(
        KPim6MessageList_QCH
        NAME KPim6MessageList
        BASE_NAME KPim6MessageList
        VERSION ${PIM_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
        ${MessageList_HEADERS}
        ${MessageList_widgets_HEADERS}
        ${MessageList_core_HEADERS}
        ${MessageList_util_HEADERS}
        LINK_QCHS
        Qt6Core_QCH
        Qt6Gui_QCH
        Qt6Widgets_QCH
        INCLUDE_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
        MESSAGELIST_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
)

ecm_install_qch_export(
    TARGETS KPim6MessageList_QCH
    FILE KPim6MessageListQchTargets.cmake
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)
set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KPim6MessageListQchTargets.cmake\")")
endif()


configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimMessageListConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageListConfig.cmake"
    INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageListConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6MessageListConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT KPim6MessageListTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KPim6MessageListTargets.cmake NAMESPACE KPim6::)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/messagelist_version.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/MessageList COMPONENT Devel
)
