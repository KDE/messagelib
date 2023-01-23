# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(viewerplugin_gui)
target_sources(viewerplugin_gui PRIVATE viewerplugin_gui.cpp viewerplugin_gui.h)
target_link_libraries(viewerplugin_gui
  KF5::MessageViewer
  KF${KF_MAJOR_VERSION}::XmlGui
  KF${KF_MAJOR_VERSION}::I18n
)

