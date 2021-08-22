/*
  SPDX-FileCopyrightText: 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>
  SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <kmime/kmime_message.h>
namespace KIdentityManagement
{
class IdentityManager;
}
class MessageFactoryTest : public QObject
{
    Q_OBJECT

public:
    explicit MessageFactoryTest(QObject *parent = nullptr);
    ~MessageFactoryTest();

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

    void cleanupTestCase();

private:
    Q_REQUIRED_RESULT KMime::Message::Ptr createPlainTestMessage();
    Q_REQUIRED_RESULT KMime::Message::Ptr loadMessageFromFile(const QString &filename);
    Q_REQUIRED_RESULT KMime::Message::Ptr createPlainTestMessageWithMultiEmails();
    Q_REQUIRED_RESULT KMime::Message::Ptr loadMessage(const QString &filename);
    KIdentityManagement::IdentityManager *mIdentMan = nullptr;
};

