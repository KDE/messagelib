# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(mndadvicedialoggui mndadvicedialoggui.cpp)
ecm_mark_as_test(mndadvicedialoggui)
target_link_libraries(mndadvicedialoggui
  Qt${QT_MAJOR_VERSION}::Core
  Qt${QT_MAJOR_VERSION}::Gui
  KF5::MessageComposer
  KF5::I18n
)
