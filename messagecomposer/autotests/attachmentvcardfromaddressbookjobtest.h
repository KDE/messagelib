/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include <QObject>

class AttachmentVcardFromAddressBookJobTest : public QObject
{
    Q_OBJECT
public:
    explicit AttachmentVcardFromAddressBookJobTest(QObject *parent = nullptr);
    ~AttachmentVcardFromAddressBookJobTest();
private Q_SLOTS:
    void testAttachmentVCardWithInvalidItem();
    void testAttachmentVCardWithValidItem();
    void testAttachmentVCardWithInvalidVCard();
    void testAttachmentVCardWithEmptyVCard();
};

