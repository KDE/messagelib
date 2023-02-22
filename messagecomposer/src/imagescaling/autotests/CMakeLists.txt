# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(imagescalingtest.cpp imagescalingtest.h
        TEST_NAME imagescalingtest
    NAME_PREFIX "messagecomposer-imagescaling-"
    LINK_LIBRARIES KPim${KF_MAJOR_VERSION}::Mime KF5::MessageComposer KF5::MessageViewer KF5::MessageCore KF5::TemplateParser
                   KF5::PimTextEdit Qt::Test KF5::IdentityManagement KF${KF_MAJOR_VERSION}::Contacts
                   KF5::AkonadiCore
)
