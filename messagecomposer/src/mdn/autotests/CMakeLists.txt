# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none

set(ECM_TEST_NAME_PREFIX "messagecomposer-mdn-")

ecm_add_test(mdnwarningwidgetjobtest.cpp mdnwarningwidgetjobtest.h
    TEST_NAME mdnwarningwidgetjobtest
    LINK_LIBRARIES Qt::Test KPim6::AkonadiCore KPim6::MessageComposer
)
