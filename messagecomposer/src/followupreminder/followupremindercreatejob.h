/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <AkonadiCore/Collection>
#include <AkonadiCore/Item>
#include <KJob>
#include <QDate>
#include <QObject>

#include "messagecomposer_export.h"

#include <memory>

namespace MessageComposer
{
class FollowupReminderCreateJobPrivate;
/**
 * @brief The FollowupReminderCreateJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT FollowupReminderCreateJob : public KJob
{
    Q_OBJECT
public:
    explicit FollowupReminderCreateJob(QObject *parent = nullptr);
    ~FollowupReminderCreateJob() override;

    void setFollowUpReminderDate(const QDate &date);

    void setOriginalMessageItemId(Akonadi::Item::Id value);

    void setMessageId(const QString &messageId);

    void setTo(const QString &to);

    void setSubject(const QString &subject);

    void setCollectionToDo(const Akonadi::Collection &collection);

    void start() override;

private Q_SLOTS:
    void slotCreateNewTodo(KJob *job);

private:
    void writeFollowupReminderInfo();

    std::unique_ptr<FollowupReminderCreateJobPrivate> const d;
};
}

