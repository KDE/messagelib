/*
   Copyright (C) 2013-2020 Laurent Montel <montel@kde.org>

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

#include "sendlaterutil.h"
#include "sendlaterutil_p.h"
#include "sendlaterinterface.h"

#include <AkonadiCore/ServerManager>

#include <QDBusInterface>
#include <servermanager.h>

using namespace MessageComposer;

QString SendLaterUtil::agentServiceName()
{
    using Akonadi::ServerManager;
    return ServerManager::agentServiceName(ServerManager::Agent, QStringLiteral("akonadi_sendlater_agent"));
}

QString SendLaterUtil::dbusPath()
{
    return QStringLiteral("/SendLaterAgent");
}

bool SendLaterUtil::sentLaterAgentWasRegistered()
{
    return org::freedesktop::Akonadi::SendLaterAgent{agentServiceName(), dbusPath(), QDBusConnection::sessionBus()}.isValid();
}

bool SendLaterUtil::sentLaterAgentEnabled()
{
    org::freedesktop::Akonadi::SendLaterAgent iface{agentServiceName(), dbusPath(), QDBusConnection::sessionBus()};
    return iface.isValid() && iface.enabledAgent();
}
