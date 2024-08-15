/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>
  SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>
#include <QObject>
namespace KIdentityManagementCore
{
class IdentityManager;
}
class MessageFactoryTest : public QObject
{
    Q_OBJECT

public:
    explicit MessageFactoryTest(QObject *parent = nullptr);
    ~MessageFactoryTest() override;

public:
    static void initMain();

private Q_SLOTS:
    void initTestCase();
    void init();

    void testCreateReplyAllAsync();

    void testCreateReplyHtmlAsync();
    void testCreateReplyUTF16Base64Async();
    void testCreateForwardAsync();

    void testCreateRedirect();
    void testCreateResend();
    void testCreateMDN();
    void testCreateRedirectToAndCC();
    void testCreateRedirectToAndCCAndBCC();

    // MultiPart
    void test_multipartAlternative_data();
    void test_multipartAlternative();

    void testCreateForwardMultiEmailsAsync();
    void testCreateReplyToAuthorAsync();
    void testCreateReplyAllWithMultiEmailsAsync();
    void testCreateReplyToListAsync();
    void testCreateReplyToAllWithUseSenderAsync();
    void testCreateReplyToAllWithUseSenderByNoSameIdentitiesAsync();
    void testCreateReplyToAllWithUseSenderAndIdentityInCCAsync();

    void testCreateReplyWithForcedCharset();

    void cleanupTestCase();

private:
    [[nodiscard]] KMime::Message::Ptr createPlainTestMessage();
    [[nodiscard]] KMime::Message::Ptr createPlainTestMessageWithMultiEmails();
    [[nodiscard]] KMime::Message::Ptr createReplyAllForMessage(KMime::Message::Ptr origMsg);
    KIdentityManagementCore::IdentityManager *mIdentMan = nullptr;
};
