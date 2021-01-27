/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "followupreminder.h"
#include "followupreminderinterface.h"

#include <AkonadiCore/ServerManager>
#include <servermanager.h>

using namespace MessageComposer;

bool FollowUpReminder::isAvailableAndEnabled()
{
    using Akonadi::ServerManager;
    org::freedesktop::Akonadi::FollowUpReminderAgent iface{
        ServerManager::agentServiceName(ServerManager::Agent, QStringLiteral("akonadi_followupreminder_agent")),
        QStringLiteral("/FollowUpReminder"),
        QDBusConnection::sessionBus()};

    return iface.isValid() && iface.enabledAgent();
}
