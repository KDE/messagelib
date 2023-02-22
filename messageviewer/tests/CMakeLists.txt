# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(viewertest_gui)
target_sources(viewertest_gui PRIVATE
    viewertest_gui.cpp
    ../src/messageviewer_debug.cpp)
target_link_libraries(viewertest_gui
  KPim${KF_MAJOR_VERSION}::Mime
  KF5::MessageViewer
  KPim${KF_MAJOR_VERSION}::GrantleeTheme
  KF${KF_MAJOR_VERSION}::I18n
  KF${KF_MAJOR_VERSION}::XmlGui
)

