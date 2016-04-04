set(adblocksettings_gui_SRCS adblocksettings_gui.cpp)
add_executable(adblocksettings_gui ${adblocksettings_gui_SRCS})
target_link_libraries(adblocksettings_gui
  KF5::MessageViewer
  KF5::ItemViews
  KF5::I18n
)

set(adblocksettingsdialog_gui_SRCS adblocksettingsdialog_gui.cpp)
add_executable(adblocksettingsdialog_gui ${adblocksettingsdialog_gui_SRCS})
target_link_libraries(adblocksettingsdialog_gui
  KF5::MessageViewer
  KF5::ItemViews
  KF5::I18n
)
if (NOT MESSAGEVIEWER_USE_QTWEBENGINE)
    set(adblockblockableitemdialog_gui_SRCS adblockblockableitemdialog_gui.cpp)
    add_executable(adblockblockableitemdialog_gui ${adblockblockableitemdialog_gui_SRCS})
    target_link_libraries(adblockblockableitemdialog_gui
        KF5::KIOCore
        KF5::MessageViewer
        KF5::Libkdepim
        Qt5::WebKitWidgets
        KF5::ItemViews
        KF5::I18n
        )
endif()

set(adblockautomaticruleslistwidget_gui_SRCS adblockautomaticruleslistwidget_gui.cpp)
add_executable(adblockautomaticruleslistwidget_gui ${adblockautomaticruleslistwidget_gui_SRCS})
target_link_libraries(adblockautomaticruleslistwidget_gui
  KF5::MessageViewer
  KF5::ItemViews
  KF5::I18n
)
