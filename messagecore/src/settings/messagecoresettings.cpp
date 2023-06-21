/* SPDX-FileCopyrightText: 2010 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "messagecoresettings.h"

using namespace MessageCore;

MessageCoreSettings *MessageCoreSettings::mSelf = nullptr;

MessageCoreSettings *MessageCoreSettings::self()
{
    if (!mSelf) {
        mSelf = new MessageCoreSettings();
        mSelf->load();
    }

    return mSelf;
}

MessageCoreSettings::MessageCoreSettings() = default;

MessageCoreSettings::~MessageCoreSettings() = default;

#include "moc_messagecoresettings.cpp"
