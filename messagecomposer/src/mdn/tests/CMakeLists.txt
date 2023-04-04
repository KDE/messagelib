# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(mndadvicedialoggui mndadvicedialoggui.cpp)
ecm_mark_as_test(mndadvicedialoggui)
target_link_libraries(mndadvicedialoggui
  Qt::Core
  Qt::Gui
  KPim6::MessageComposer
  KF6::I18n
)
