/*
    This file is part of KMail.

    SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#pragma once

#include "globalsettings_messageviewer.h"

class QTimer;

namespace MessageViewer
{
/**
 * @brief The MessageViewerSettings class
 */
class MESSAGEVIEWER_EXPORT MessageViewerSettings : public MessageViewer::MessageViewerSettingsBase
{
    Q_OBJECT
public:
    static MessageViewerSettings *self();

    /** Call this slot instead of directly KConfig::sync() to
        minimize the overall config writes. Calling this slot will
        schedule a sync of the application config file using a timer, so
        that many consecutive calls can be condensed into a single
        sync, which is more efficient. */
    void requestSync();

private Q_SLOTS:
    MESSAGEVIEWER_NO_EXPORT void slotSyncNow();

private:
    MESSAGEVIEWER_NO_EXPORT MessageViewerSettings();
    ~MessageViewerSettings() override;
    static MessageViewerSettings *mSelf;

    QTimer *mConfigSyncTimer = nullptr;
};
}
