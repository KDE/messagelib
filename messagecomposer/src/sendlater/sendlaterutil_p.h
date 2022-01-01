/*
   SPDX-FileCopyrightText: 2013-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Akonadi/ServerManager>

namespace MessageComposer
{
namespace SendLaterUtil
{
Q_REQUIRED_RESULT QString agentServiceName();
Q_REQUIRED_RESULT QString dbusPath();
}
}

