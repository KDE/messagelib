# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(viewertest_gui)
target_sources(viewertest_gui PRIVATE
    viewertest_gui.cpp
    ../src/messageviewer_debug.cpp)
target_link_libraries(viewertest_gui
  KPim6::Mime
  KPim6::MessageViewer
  KPim6::GrantleeTheme
  KF6::I18n
  KF6::XmlGui
  Qt::WebEngineWidgets
)

