# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

set(ECM_TEST_NAME_PREFIX "messageviewer-webengine-")

ecm_test_get_name_prefix(test_name_prefix)
ecm_add_test(webengineaccesskeyanchortest.cpp webengineaccesskeyanchortest.h ../webengineaccesskeyanchor.cpp ../webengineaccesskeyutils.cpp
    TEST_NAME webengineaccesskeyanchortest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer
)
# needed for the CI to run webengine-related tests
# the CI runs as root, and webengine insists on disabling sandboxing in that case
set_tests_properties(${test_name_prefix}webengineaccesskeyanchortest PROPERTIES ENVIRONMENT "QTWEBENGINE_DISABLE_SANDBOX=1")

ecm_add_test(webengineaccesskeyanchorfromhtmltest.cpp webengineaccesskeyanchorfromhtmltest.h ../webengineaccesskeyanchor.cpp ../webengineaccesskeyutils.cpp
    TEST_NAME webengineaccesskeyanchorfromhtmltest
    LINK_LIBRARIES Qt::Test KPim6::WebEngineViewer  Qt::WebEngineWidgets
)
# needed for the CI to run webengine-related tests
# the CI runs as root, and webengine insists on disabling sandboxing in that case
set_tests_properties(${test_name_prefix}webengineaccesskeyanchorfromhtmltest PROPERTIES ENVIRONMENT "QTWEBENGINE_DISABLE_SANDBOX=1")
