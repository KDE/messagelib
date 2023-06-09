# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(imagescalingtest.cpp imagescalingtest.h
        TEST_NAME imagescalingtest
    NAME_PREFIX "messagecomposer-imagescaling-"
    LINK_LIBRARIES KPim${KF_MAJOR_VERSION}::Mime KPim${KF_MAJOR_VERSION}::MessageComposer
                   KPim${KF_MAJOR_VERSION}::PimTextEdit Qt::Test KPim${KF_MAJOR_VERSION}::IdentityManagement KF${KF_MAJOR_VERSION}::Contacts
                   KPim${KF_MAJOR_VERSION}::AkonadiCore
)
