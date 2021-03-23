/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <AkonadiCore/Collection>
#include <KMime/Message>
#include <QObject>
namespace KIdentityManagement
{
class IdentityManager;
}

namespace MessageComposer
{
class MessageFactoryForwardJob : public QObject
{
    Q_OBJECT
public:
    explicit MessageFactoryForwardJob(QObject *parent = nullptr);
    ~MessageFactoryForwardJob() override;
    void start();
    void setMsg(const KMime::Message::Ptr &msg);

    void setTemplate(const QString &tmpl);

    void setSelection(const QString &selection);

    void setOrigMsg(const KMime::Message::Ptr &origMsg);

    void setIdentityManager(KIdentityManagement::IdentityManager *identityManager);

    void setCollection(const Akonadi::Collection &collection);

Q_SIGNALS:
    void forwardDone(const KMime::Message::Ptr &msg);

private:
    void slotParsingDone();

    QString mSelection;
    QString mTemplate;
    KMime::Message::Ptr mMsg = nullptr;
    KMime::Message::Ptr mOrigMsg = nullptr;
    Akonadi::Collection mCollection;
    KIdentityManagement::IdentityManager *mIdentityManager = nullptr;
};
}

