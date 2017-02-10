/*
  Copyright (C) 2010 Klaralvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Copyright (c) 2010 Leo Franchi <lfranchi@kde.org>
  Copyright (c) 2017 Laurent Montel <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#ifndef MESSAGEFACTORYNG_TEST_H
#define MESSAGEFACTORYNG_TEST_H

#include <QObject>
#include <kmime/kmime_message.h>
namespace KIdentityManagement
{
class IdentityManager;
}
class MessageFactoryTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void testCreateReplyAll();
    void testCreateReplyAllAsync();

    void testCreateReplyHtml();
    void testCreateReplyUTF16Base64();
    void testCreateForward();
    void testCreateForwardAsync();

    void testCreateRedirect();
    void testCreateResend();
    void testCreateMDN();
    void testCreateRedirectToAndCC();
    void testCreateRedirectToAndCCAndBCC();

    //MultiPart
    void test_multipartAlternative_data();
    void test_multipartAlternative();

    void testCreateForwardMultiEmailsAsync();
    void testCreateForwardMultiEmails();
    void testCreateReplyToAuthor();
    void testCreateReplyAllWithMultiEmails();
    void testCreateReplyToList();
    void testCreateReplyToAllWithUseSender();
    void testCreateReplyToAllWithUseSenderByNoSameIdentities();
    void testCreateReplyToAllWithUseSenderAndIdentityInCC();

    void cleanupTestCase();
private:
    KMime::Message::Ptr createPlainTestMessage();
    KMime::Message::Ptr loadMessageFromFile(const QString &filename);
    KMime::Message::Ptr createPlainTestMessageWithMultiEmails();
    KMime::Message::Ptr loadMessage(const QString &filename);
    KIdentityManagement::IdentityManager *mIdentMan;
};

#endif
