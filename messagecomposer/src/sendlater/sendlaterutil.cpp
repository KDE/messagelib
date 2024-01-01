/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlaterutil.h"
#include "sendlaterinterface.h"
#include "sendlaterutil_p.h"

#include <Akonadi/ServerManager>

#include <Akonadi/ServerManager>

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
