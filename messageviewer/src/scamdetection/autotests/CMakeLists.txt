# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(scamattributetest.cpp scamattributetest.h ../scamattribute.cpp
    TEST_NAME scamattributetest
    NAME_PREFIX "messageviewer-scamdetection-"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::AkonadiCore
    )

ecm_add_test(scamdetectionwebenginetest.cpp scamdetectionwebenginetest.h
    TEST_NAME scamdetectionwebenginetest
    NAME_PREFIX "messageviewer-scamwebenginedetection-"
    LINK_LIBRARIES KF5::MessageViewer Qt${QT_MAJOR_VERSION}::Test  Qt${QT_MAJOR_VERSION}::WebEngineWidgets
    )

ecm_add_test(scamdetectioninfotest.cpp scamdetectioninfotest.h
    TEST_NAME scamdetectioninfotest
    NAME_PREFIX "messageviewer-scamdetection-"
    LINK_LIBRARIES KF5::MessageViewer Qt${QT_MAJOR_VERSION}::Test  Qt${QT_MAJOR_VERSION}::WebEngineWidgets
    )
