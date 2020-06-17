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
        QDBusConnection::sessionBus()
    };

    return iface.isValid() && iface.enabledAgent();
}
