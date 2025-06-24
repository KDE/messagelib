/*
   SPDX-FileCopyrightText: 2020 Daniel Vrátil <dvratil@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "followupreminder.h"

#include "followupreminderinterface.h"

#include <Akonadi/ServerManager>

using namespace MessageComposer;
using namespace Qt::Literals::StringLiterals;

bool FollowUpReminder::isAvailableAndEnabled()
{
    using Akonadi::ServerManager;
    org::freedesktop::Akonadi::FollowUpReminderAgent iface{ServerManager::agentServiceName(ServerManager::Agent, u"akonadi_followupreminder_agent"_s),
                                                           u"/FollowUpReminder"_s,
                                                           QDBusConnection::sessionBus()};

    return iface.isValid() && iface.enabledAgent();
}
