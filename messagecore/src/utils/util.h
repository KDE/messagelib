/* SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messagecore_export.h"

#include <KIdentityManagement/Identity>

namespace KIdentityManagement
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
Q_REQUIRED_RESULT MESSAGECORE_EXPORT const KIdentityManagement::Identity &
identityForMessage(KMime::Message *msg, const KIdentityManagement::IdentityManager *const identMgr, uint folderIdentityId);
}
}

