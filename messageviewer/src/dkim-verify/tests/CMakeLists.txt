# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(dkimmanagerkeydialog_gui)
target_sources(dkimmanagerkeydialog_gui PRIVATE dkimmanagerkeydialog_gui.cpp)
target_link_libraries(
    dkimmanagerkeydialog_gui
    Qt::Network
    KPim6::MessageViewer
)
