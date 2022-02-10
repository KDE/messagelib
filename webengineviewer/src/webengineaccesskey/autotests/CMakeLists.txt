# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(webengineaccesskeyanchortest.cpp webengineaccesskeyanchortest.h ../webengineaccesskeyanchor.cpp ../webengineaccesskeyutils.cpp
    TEST_NAME webengineaccesskeyanchortest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::WebEngineViewer
)

ecm_add_test(webengineaccesskeyanchorfromhtmltest.cpp webengineaccesskeyanchorfromhtmltest.h ../webengineaccesskeyanchor.cpp ../webengineaccesskeyutils.cpp
    TEST_NAME webengineaccesskeyanchorfromhtmltest
    NAME_PREFIX "messageviewer-webengine-"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test KF5::WebEngineViewer  Qt${QT_MAJOR_VERSION}::WebEngineWidgets
)
