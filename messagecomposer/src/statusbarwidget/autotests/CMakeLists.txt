# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
add_executable(statusbarlabeltoggledstatetest statusbarlabeltoggledstatetest.cpp)
ecm_mark_as_test(kmail-statusbarlabeltoggledstatetest)
target_link_libraries( statusbarlabeltoggledstatetest Qt${QT_MAJOR_VERSION}::Test KF5::KIOWidgets KF5::I18n KF5::MessageComposer)

