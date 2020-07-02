/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_SENDLATERUTIL_P_H
#define MESSAGECOMPOSER_SENDLATERUTIL_P_H

#include <AkonadiCore/ServerManager>
#include <AkonadiCore/servermanager.h>

namespace MessageComposer {
namespace SendLaterUtil {
QString agentServiceName();
QString dbusPath();
}
}

#endif
