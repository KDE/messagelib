/*
   SPDX-FileCopyrightText: 2013-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlaterutil.h"

#include "sendlaterinterface.h"
#include "sendlaterutil_p.h"

#include <Akonadi/ServerManager>

using namespace Qt::Literals::StringLiterals;
using namespace MessageComposer;

QString SendLaterUtil::agentServiceName()
{
    using Akonadi::ServerManager;
    return ServerManager::agentServiceName(ServerManager::Agent, u"akonadi_sendlater_agent"_s);
}

QString SendLaterUtil::dbusPath()
{
    return u"/SendLaterAgent"_s;
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
