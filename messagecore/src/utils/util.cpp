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
    const auto to = msg->to(KMime::CreatePolicy::DontCreate);
    const auto cc = msg->cc(KMime::CreatePolicy::DontCreate);
    const auto &identity = identMgr->identityForAddress((to ? to->asUnicodeString() : QString()) + ", "_L1 + (cc ? cc->asUnicodeString() : QString()));

    if (identity.isNull()) {
        return identMgr->identityForUoidOrDefault(folderIdentityId);
    }

    return identity;
}
