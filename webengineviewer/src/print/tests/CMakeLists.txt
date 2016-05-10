set(printpreviewdialog_gui_test_SRCS
    printpreviewdialog_gui.cpp
    ../printpreviewdialog.cpp
    ../printpreviewpageviewer.cpp
    )

add_executable(printpreviewdialog_gui ${printpreviewdialog_gui_test_SRCS})

target_link_libraries(printpreviewdialog_gui
    Qt5::Widgets
    Qt5::Widgets
    Qt5::Gui
    KF5::I18n
    KF5::WidgetsAddons
    Poppler::Qt5
    KF5::ConfigCore
    )
