/* SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "util.h"

#include <KIdentityManagement/IdentityManager>

#include <KMime/Message>

using namespace MessageCore;

const KIdentityManagement::Identity &
Util::identityForMessage(KMime::Message *msg, const KIdentityManagement::IdentityManager *const identMgr, uint folderIdentityId)
{
    const auto &identity = identMgr->identityForAddress(msg->to()->asUnicodeString() + QLatin1String(", ") + msg->cc()->asUnicodeString());

    if (identity.isNull()) {
        return identMgr->identityForUoidOrDefault(folderIdentityId);
    }

    return identity;
}
