set(printpreviewdialog_gui_test_SRCS
    printpreviewdialog_gui.cpp
    )

add_executable(printpreviewdialog_gui ${printpreviewdialog_gui_test_SRCS})

target_link_libraries(printpreviewdialog_gui
    Qt5::Widgets
    )
