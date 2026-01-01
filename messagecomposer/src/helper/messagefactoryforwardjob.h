/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <Akonadi/Collection>
#include <KMime/Message>
#include <QObject>
namespace KIdentityManagementCore
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
    void setMsg(const std::shared_ptr<KMime::Message> &msg);

    void setTemplate(const QString &tmpl);

    void setSelection(const QString &selection);

    void setOrigMsg(const std::shared_ptr<KMime::Message> &origMsg);

    void setIdentityManager(KIdentityManagementCore::IdentityManager *identityManager);

    void setCollection(const Akonadi::Collection &collection);

Q_SIGNALS:
    void forwardDone(const std::shared_ptr<KMime::Message> &msg);

private:
    void slotParsingDone();

    QString mSelection;
    QString mTemplate;
    std::shared_ptr<KMime::Message> mMsg = nullptr;
    std::shared_ptr<KMime::Message> mOrigMsg = nullptr;
    Akonadi::Collection mCollection;
    KIdentityManagementCore::IdentityManager *mIdentityManager = nullptr;
};
}
