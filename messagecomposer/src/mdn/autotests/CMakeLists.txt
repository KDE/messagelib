# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(mdnwarningwidgetjobtest.cpp
    TEST_NAME mdnwarningwidgetjobtest
    NAME_PREFIX "messagecomposer-mdn-"
    LINK_LIBRARIES Qt::Test KF5::AkonadiCore KF5::MessageComposer
)
