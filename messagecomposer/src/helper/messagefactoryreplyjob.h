/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <Akonadi/Collection>
#include <KIdentityManagementCore/IdentityManager>
#include <KMime/Message>
#include <QObject>

namespace MessageComposer
{
class MessageFactoryReplyJob : public QObject
{
    Q_OBJECT
public:
    explicit MessageFactoryReplyJob(QObject *parent = nullptr);
    ~MessageFactoryReplyJob() override;

    void start();

    void setMsg(const KMime::Message::Ptr &msg);

    void setTemplate(const QString &tmpl);

    void setSelection(const QString &selection);

    void setOrigMsg(const KMime::Message::Ptr &origMsg);

    void setIdentityManager(KIdentityManagementCore::IdentityManager *identityManager);

    void setReplyAll(bool replyAll);

    void setCollection(const Akonadi::Collection &collection);

    void setReplyAsHtml(bool replyAsHtml);

Q_SIGNALS:
    void replyDone(const KMime::Message::Ptr &msg, bool replyAll);

private:
    void slotReplyDone();
    QString mSelection;
    QString mTemplate;
    KMime::Message::Ptr mMsg = nullptr;
    KMime::Message::Ptr mOrigMsg = nullptr;
    Akonadi::Collection mCollection;
    bool mReplyAll = false;
    bool mReplyAsHtml = false;
    KIdentityManagementCore::IdentityManager *mIdentityManager = nullptr;
};
}
