/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_FOLLOWUPREMINDERCREATEJOB_H
#define MESSAGECOMPOSER_FOLLOWUPREMINDERCREATEJOB_H

#include <QObject>
#include <QDate>
#include <AkonadiCore/Item>
#include <AkonadiCore/Collection>
#include <KJob>

#include "messagecomposer_export.h"

#include <memory>

namespace MessageComposer {
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

#endif
