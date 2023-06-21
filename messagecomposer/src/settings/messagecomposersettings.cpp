/*
    This file is part of KMail.

    SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#include "messagecomposersettings.h"
#include <QTimer>

using namespace MessageComposer;

MessageComposerSettings *MessageComposerSettings::mSelf = nullptr;

MessageComposerSettings *MessageComposerSettings::self()
{
    if (!mSelf) {
        mSelf = new MessageComposerSettings();
        mSelf->load();
    }

    return mSelf;
}

MessageComposerSettings::MessageComposerSettings()
{
    mConfigSyncTimer = new QTimer(this);
    mConfigSyncTimer->setSingleShot(true);
    connect(mConfigSyncTimer, &QTimer::timeout, this, &MessageComposerSettings::slotSyncNow);
}

void MessageComposerSettings::requestSync()
{
    if (!mConfigSyncTimer->isActive()) {
        mConfigSyncTimer->start(0);
    }
}

void MessageComposerSettings::slotSyncNow()
{
    config()->sync();
}

MessageComposerSettings::~MessageComposerSettings() = default;

#include "moc_messagecomposersettings.cpp"
