

option(KDEPIM_FOLDEROPEN_PROFILE "Enable debug output for timing folder loading" FALSE)
configure_file(config-messagelist.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-messagelist.h)
add_library(KF5MessageList)
add_library(KF5::MessageList ALIAS KF5MessageList)

add_definitions(-DTRANSLATION_DOMAIN=\"libmessagelist\")

add_subdirectory(pics)
if(BUILD_TESTING)
	add_subdirectory(core/widgets/autotests)
endif()
target_sources(KF5MessageList PRIVATE
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
    core/twofingertap.cpp
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
    )
ecm_qt_declare_logging_category(KF5MessageList HEADER messagelist_debug.h IDENTIFIER MESSAGELIST_LOG CATEGORY_NAME org.kde.pim.messagelist
        DESCRIPTION "messagelib (messagelist)"
        OLD_CATEGORY_NAMES log_messagelist
        EXPORT MESSAGELIB
    )

kconfig_add_kcfg_files(KF5MessageList
    core/messagelistsettings.kcfgc
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KF5MessageList PROPERTIES UNITY_BUILD ON)
endif()

generate_export_header(KF5MessageList BASE_NAME messagelist)

target_include_directories(KF5MessageList INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/MessageList/;${KDE_INSTALL_INCLUDEDIR_KF5}/messagelist>")

target_link_libraries(KF5MessageList
    PUBLIC
    KF5::AkonadiCore
    KF5::AkonadiMime
    KF5::Mime
    PRIVATE
    KF5::KIOCore
    KF5::MessageCore
    KF5::PimCommonAkonadi
    KF5::AkonadiSearchPIM
    KF5::XmlGui
    KF5::IconThemes
    KF5::Completion
    KF5::I18n
    KF5::TextWidgets
    )

set_target_properties(KF5MessageList PROPERTIES
    VERSION ${MESSAGELIST_VERSION}
    SOVERSION ${MESSAGELIST_SOVERSION}
    EXPORT_NAME MessageList
    )

install(TARGETS
    KF5MessageList
    EXPORT KF5MessageListTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
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
    LIB_NAME KF5MessageList
    DEPS "AkonadiCore AkonadiMime KMime" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageList
    )


install(FILES
    ${MessageList_Camelcasewidgets_HEADERS}
    ${MessageList_CamelCase_HEADERS}
    ${MessageList_Camelcasecore_HEADERS}
    ${MessageList_Camelcaseutil_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/MessageList
    COMPONENT Devel
    )

install(FILES
    ${MessageList_HEADERS}
    ${MessageList_widgets_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/messagelist_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/messagelistsettings.h
    ${MessageList_core_HEADERS}
    ${MessageList_util_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/messagelist
    COMPONENT Devel
    )
install(FILES
    ${PRI_FILENAME}
    DESTINATION ${ECM_MKSPECS_INSTALL_DIR})


