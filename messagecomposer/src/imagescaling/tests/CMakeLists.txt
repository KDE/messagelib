# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(imagescaling_gui imagescaling_gui.cpp imagescaling_gui.h)
target_link_libraries(imagescaling_gui
  KF5::KIOCore
  KF5::MessageComposer
  KF5::I18n
)


