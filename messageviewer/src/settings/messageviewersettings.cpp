/*
    This file is part of KMail.

    SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#include "messageviewersettings.h"
#include <QTimer>

using namespace MessageViewer;

MessageViewerSettings *MessageViewerSettings::mSelf = nullptr;

MessageViewerSettings *MessageViewerSettings::self()
{
    if (!mSelf) {
        mSelf = new MessageViewerSettings();
        mSelf->load();
    }

    return mSelf;
}

MessageViewerSettings::MessageViewerSettings()
{
    mConfigSyncTimer = new QTimer(this);
    mConfigSyncTimer->setSingleShot(true);
    connect(mConfigSyncTimer, &QTimer::timeout, this, &MessageViewerSettings::slotSyncNow);
}

void MessageViewerSettings::requestSync()
{
    if (!mConfigSyncTimer->isActive()) {
        mConfigSyncTimer->start(0);
    }
}

void MessageViewerSettings::slotSyncNow()
{
    config()->sync();
}

MessageViewerSettings::~MessageViewerSettings() = default;

#include "moc_messageviewersettings.cpp"
