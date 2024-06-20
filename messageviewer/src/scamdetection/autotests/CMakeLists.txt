# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(scamattributetest.cpp scamattributetest.h ../scamattribute.cpp
    TEST_NAME scamattributetest
    NAME_PREFIX "messageviewer-scamdetection-"
    LINK_LIBRARIES Qt::Test KPim6::AkonadiCore
)

ecm_add_test(scamdetectionwebenginetest.cpp scamdetectionwebenginetest.h
    TEST_NAME scamdetectionwebenginetest
    NAME_PREFIX "messageviewer-scamwebenginedetection-"
    LINK_LIBRARIES KPim6::MessageViewer Qt::Test  Qt::WebEngineWidgets
)

ecm_add_test(scamdetectioninfotest.cpp scamdetectioninfotest.h
    TEST_NAME scamdetectioninfotest
    NAME_PREFIX "messageviewer-scamdetection-"
    LINK_LIBRARIES KPim6::MessageViewer Qt::Test  Qt::WebEngineWidgets
)
