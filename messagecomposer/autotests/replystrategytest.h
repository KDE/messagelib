/*
  SPDX-FileCopyrightText: 2019 Glen Ditchfield <GJDitchfield@acm.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KMime/Message>

#include <QObject>

#include <MessageComposer/MessageFactoryNG>

namespace KIdentityManagementCore
{
class IdentityManager;
}

class ReplyStrategyTest : public QObject
{
    Q_OBJECT

public:
    explicit ReplyStrategyTest(QObject *parent = nullptr);
    ~ReplyStrategyTest() override;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testReply_data();
    void testReply();

private:
    KMime::Message::Ptr makeReply(const KMime::Message::Ptr &original, const MessageComposer::ReplyStrategy strategy);

    KIdentityManagementCore::IdentityManager *mIdentityManager = nullptr;
};
