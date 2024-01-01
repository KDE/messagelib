/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <Akonadi/ServerManager>

namespace MessageComposer
{
namespace SendLaterUtil
{
[[nodiscard]] QString agentServiceName();
[[nodiscard]] QString dbusPath();
}
}
