/* SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "util.h"

#include <KIdentityManagementCore/IdentityManager>

#include <KMime/Message>

using namespace Qt::Literals::StringLiterals;

using namespace MessageCore;

const KIdentityManagementCore::Identity &
Util::identityForMessage(KMime::Message *msg, const KIdentityManagementCore::IdentityManager *const identMgr, uint folderIdentityId)
{
    const auto &identity = identMgr->identityForAddress(msg->to()->asUnicodeString() + ", "_L1 + msg->cc()->asUnicodeString());

    if (identity.isNull()) {
        return identMgr->identityForUoidOrDefault(folderIdentityId);
    }

    return identity;
}
