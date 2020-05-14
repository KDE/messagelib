/*
   Copyright (C) 2014-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "followupremindercreatejob.h"
#include "followupreminderinterface.h"
#include "messagecomposer_debug.h"

#include <KCalendarCore/Todo>
#include <KLocalizedString>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/ServerManager>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

using namespace MessageComposer;
class MessageComposer::FollowupReminderCreateJobPrivate
{
public:
    Akonadi::Collection mCollection;
    QDate mFollowupDate;
    Akonadi::Item::Id mOriginalMessageItemId = -1;
    Akonadi::Item::Id mTodoId = -1;
    QString mMessageId;
    QString mSubject;
    QString mTo;
};

FollowupReminderCreateJob::FollowupReminderCreateJob(QObject *parent)
    : KJob(parent)
    , d(new MessageComposer::FollowupReminderCreateJobPrivate)
{
}

FollowupReminderCreateJob::~FollowupReminderCreateJob() = default;

void FollowupReminderCreateJob::setFollowUpReminderDate(const QDate &date)
{
    d->mFollowupDate = date;
}

void FollowupReminderCreateJob::setOriginalMessageItemId(Akonadi::Item::Id value)
{
    d->mOriginalMessageItemId = value;
}

void FollowupReminderCreateJob::setMessageId(const QString &messageId)
{
    d->mMessageId = messageId;
}

void FollowupReminderCreateJob::setTo(const QString &to)
{
    d->mTo = to;
}

void FollowupReminderCreateJob::setSubject(const QString &subject)
{
    d->mSubject = subject;
}

void FollowupReminderCreateJob::setCollectionToDo(const Akonadi::Collection &collection)
{
    d->mCollection = collection;
}

void FollowupReminderCreateJob::start()
{
    if (!d->mMessageId.isEmpty() && d->mFollowupDate.isValid() && !d->mTo.isEmpty()) {
        if (d->mCollection.isValid()) {
            KCalendarCore::Todo::Ptr todo(new KCalendarCore::Todo);
            todo->setSummary(i18n("Wait answer from \"%1\" send to \"%2\"", d->mSubject, d->mTo));
            todo->setDtDue(QDateTime(d->mFollowupDate, QTime(0, 0, 0)));
            Akonadi::Item newTodoItem;
            newTodoItem.setMimeType(KCalendarCore::Todo::todoMimeType());
            newTodoItem.setPayload<KCalendarCore::Todo::Ptr>(todo);

            Akonadi::ItemCreateJob *createJob = new Akonadi::ItemCreateJob(newTodoItem, d->mCollection);
            connect(createJob, &Akonadi::ItemCreateJob::result, this, &FollowupReminderCreateJob::slotCreateNewTodo);
        } else {
            writeFollowupReminderInfo();
        }
    } else {
        qCWarning(MESSAGECOMPOSER_LOG) << "FollowupReminderCreateJob info not valid!";
        emitResult();
        return;
    }
}

void FollowupReminderCreateJob::slotCreateNewTodo(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Error during create new Todo " << job->errorString();
        setError(job->error());
        setErrorText(i18n("Failed to store a new reminder: an error occurred while trying to create a new Todo in your calendar: %1", job->errorString()));
        emitResult();
        return;
    }

    Akonadi::ItemCreateJob *createJob = qobject_cast<Akonadi::ItemCreateJob *>(job);
    d->mTodoId = createJob->item().id();
    writeFollowupReminderInfo();
}

void FollowupReminderCreateJob::writeFollowupReminderInfo()
{
    std::unique_ptr<org::freedesktop::Akonadi::FollowUpReminderAgent> iface{
        new org::freedesktop::Akonadi::FollowUpReminderAgent{
            Akonadi::ServerManager::agentServiceName(Akonadi::ServerManager::Agent, QStringLiteral("akonadi_followupreminder_agent")),
            QStringLiteral("/FollowUpReminder"),
            QDBusConnection::sessionBus()
    }};

    if (!iface->isValid()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "The FollowUpReminder agent is not running!";
        return;
    }

    auto call = iface->addReminder(d->mMessageId, d->mOriginalMessageItemId, d->mTo, d->mSubject, d->mFollowupDate, d->mTodoId);
    auto wait = new QDBusPendingCallWatcher{call, this};
    connect(wait, &QDBusPendingCallWatcher::finished,
            this, [this, iface_ = std::move(iface)](QDBusPendingCallWatcher *watcher) mutable {
                auto iface = std::move(iface_);
                watcher->deleteLater();

                const QDBusPendingReply<void> reply = *watcher;
                if (reply.isError()) {
                    qCWarning(MESSAGECOMPOSER_LOG) << "Failed to write the new reminder, agent replied" << reply.error().message();
                    setError(KJob::UserDefinedError);
                    setErrorText(i18n("Failed to store a new reminder: %1", reply.error().message()));
                }

                emitResult();
            });
}
