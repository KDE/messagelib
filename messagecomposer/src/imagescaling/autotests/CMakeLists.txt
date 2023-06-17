# SPDX-License-Identifier: CC0-1.0
# SPDX-FileCopyrightText: none
ecm_add_test(imagescalingtest.cpp imagescalingtest.h
        TEST_NAME imagescalingtest
    NAME_PREFIX "messagecomposer-imagescaling-"
    LINK_LIBRARIES KPim6::Mime KPim6::MessageComposer KPim6::MessageCore KPim6::TemplateParser
                   KPim6::PimTextEdit Qt::Test KPim6::IdentityManagementCore KF6::Contacts
                   KPim6::AkonadiCore
)
