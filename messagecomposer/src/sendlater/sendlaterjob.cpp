/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlaterjob.h"
#include "messagecomposer_debug.h"
#include "sendlaterinterface.h"
#include "sendlaterutil_p.h"

#include <KLocalizedString>

#include <QDBusPendingCallWatcher>

#include <memory>

using namespace MessageComposer;

SendLaterJob::SendLaterJob(QObject *parent)
    : KJob(parent)
{
}

void SendLaterJob::start()
{
    std::unique_ptr<org::freedesktop::Akonadi::SendLaterAgent> iface{
        new org::freedesktop::Akonadi::SendLaterAgent{SendLaterUtil::agentServiceName(), SendLaterUtil::dbusPath(), QDBusConnection::sessionBus()}};
    if (!iface->isValid()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "The SendLater agent is not running!";
        setError(SendLaterJob::AgentNotAvailable);
        setErrorText(getErrorString(SendLaterJob::AgentNotAvailable, i18n("The Send Later agent is not running.")));
        emitResult();
        return;
    }

    auto reply = doCall(iface.get());

    auto watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, iface_ = std::move(iface)](QDBusPendingCallWatcher *call) mutable {
        auto iface = std::move(iface_);
        call->deleteLater();
        QDBusPendingReply<void> reply = *call;
        if (reply.isError()) {
            qCWarning(MESSAGECOMPOSER_LOG) << "SendLater agent DBus call failed:" << reply.error().message();
            setError(SendLaterJob::CallFailed);
            setErrorText(getErrorString(SendLaterJob::CallFailed, reply.error().message()));
        }

        emitResult();
    });
}

#include "moc_sendlaterjob.cpp"
