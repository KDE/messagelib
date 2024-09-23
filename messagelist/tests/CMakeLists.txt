# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

add_executable(searchlinecommand_gui
    searchlinecommandwidgetgui.h searchlinecommandwidgetgui.cpp)
target_sources(searchlinecommand_gui PRIVATE
    searchlinecommand_gui.cpp)
target_link_libraries(searchlinecommand_gui
    KPim6::MessageList
    Qt::Widgets
)
