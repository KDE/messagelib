# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(mdnwarningwidgetjobtest.cpp mdnwarningwidgetjobtest.h
    TEST_NAME mdnwarningwidgetjobtest
    NAME_PREFIX "messagecomposer-mdn-"
    LINK_LIBRARIES Qt::Test KPim6::AkonadiCore KPim6::MessageComposer
)
