/* SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messagecore_export.h"

#include <KIdentityManagementCore/Identity>

namespace KIdentityManagementCore
{
class IdentityManager;
}

namespace KMime
{
class Message;
}

namespace MessageCore
{
/**
 * This namespace contain helper functions
 */
namespace Util
{
[[nodiscard]] MESSAGECORE_EXPORT const KIdentityManagementCore::Identity &
identityForMessage(KMime::Message *msg, const KIdentityManagementCore::IdentityManager *const identMgr, uint folderIdentityId);
}
}
