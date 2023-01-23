# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(imagescaling_gui imagescaling_gui.cpp imagescaling_gui.h)
target_link_libraries(imagescaling_gui
  KF${KF_MAJOR_VERSION}::KIOCore
  KF5::MessageComposer
  KF${KF_MAJOR_VERSION}::I18n
)


