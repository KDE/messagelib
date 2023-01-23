# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(findbarbasetest.cpp findbarbasetest.h
    TEST_NAME findbarbasetest
    NAME_PREFIX "webengineviewer-findbar-"
    LINK_LIBRARIES KF5WebEngineViewer Qt::Test Qt::Widgets Qt::Gui KF${KF_MAJOR_VERSION}::Completion KF${KF_MAJOR_VERSION}::I18n KF${KF_MAJOR_VERSION}::ConfigWidgets KF5::PimCommon
)
set_property(TARGET findbarbasetest PROPERTY INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/webengineviewer ${CMAKE_BINARY_DIR}/webengineviewer/src)
