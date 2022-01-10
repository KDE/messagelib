# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(findbarbasetest.cpp
    TEST_NAME findbarbasetest
    NAME_PREFIX "webengineviewer-findbar-"
    LINK_LIBRARIES KF5WebEngineViewer Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Widgets Qt${QT_MAJOR_VERSION}::Gui KF5::Completion KF5::I18n KF5::ConfigWidgets KF5::PimCommon
)
set_property(TARGET findbarbasetest PROPERTY INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/webengineviewer ${CMAKE_BINARY_DIR}/webengineviewer/src)
