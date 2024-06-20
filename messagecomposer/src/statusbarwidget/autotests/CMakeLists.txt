# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(statusbarlabeltoggledstatetest statusbarlabeltoggledstatetest.cpp)
ecm_mark_as_test(kmail-statusbarlabeltoggledstatetest)
target_link_libraries(statusbarlabeltoggledstatetest Qt::Test KF6::KIOWidgets KF6::I18n KPim6::MessageComposer)

