# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(statusbarlabeltoggledstatetest statusbarlabeltoggledstatetest.cpp)
ecm_mark_as_test(kmail-statusbarlabeltoggledstatetest)
target_link_libraries( statusbarlabeltoggledstatetest Qt::Test KF${KF_MAJOR_VERSION}::KIOWidgets KF${KF_MAJOR_VERSION}::I18n KF5::MessageComposer)

