/*
    This file is part of KMail.

    SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#pragma once

#include "messagecomposer_export.h"
#include "messagecomposersettings_base.h"

class QTimer;

namespace MessageComposer
{
/**
 * @brief The MessageComposerSettings class
 */
class MESSAGECOMPOSER_EXPORT MessageComposerSettings : public MessageComposer::MessageComposerSettingsBase
{
    Q_OBJECT
public:
    static MessageComposerSettings *self();

    /** Call this slot instead of directly @ref KConfig::sync() to
      minimize the overall config writes. Calling this slot will
      schedule a sync of the application config file using a timer, so
      that many consecutive calls can be condensed into a single
      sync, which is more efficient. */
    void requestSync();

private Q_SLOTS:
    void slotSyncNow();

private:
    MessageComposerSettings();
    ~MessageComposerSettings() override;
    static MessageComposerSettings *mSelf;

    QTimer *mConfigSyncTimer = nullptr;
};
}
