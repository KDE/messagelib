# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(mdnstateattributetest.cpp
    TEST_NAME mdnattributetest
    NAME_PREFIX "mailcommon-mdn-"
    LINK_LIBRARIES Qt${QT_MAJOR_VERSION}::Test Qt${QT_MAJOR_VERSION}::Gui KF5::AkonadiCore KF5::MailCommon
)
