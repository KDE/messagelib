/*
   Copyright (C) 2020 Daniel Vrátil <dvratil@kde.org>

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

#include "sendlaterjob.h"
#include "sendlaterinterface.h"
#include "sendlaterutil_p.h"
#include "messagecomposer_debug.h"

#include <KLocalizedString>

#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

#include <memory>

using namespace MessageComposer;

SendLaterJob::SendLaterJob(QObject *parent)
    : KJob(parent)
{
}

void SendLaterJob::start()
{
    std::unique_ptr<org::freedesktop::Akonadi::SendLaterAgent> iface{
        new org::freedesktop::Akonadi::SendLaterAgent{
            SendLaterUtil::agentServiceName(), SendLaterUtil::dbusPath(),
            QDBusConnection::sessionBus()
        }};
    if (!iface->isValid()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "The SendLater agent is not running!";
        setError(SendLaterJob::AgentNotAvailable);
        setErrorText(getErrorString(SendLaterJob::AgentNotAvailable, i18n("The Send Later agent is not running.")));
        emitResult();
        return;
    }

    auto reply = doCall(iface.get());

    auto watcher = new QDBusPendingCallWatcher(reply);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, [this, iface_ = std::move(iface)](QDBusPendingCallWatcher *call) mutable {
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
