/* SPDX-FileCopyrightText: 2010 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include "messagecore_export.h"

#include "globalsettings_messagecore.h"

namespace MessageCore
{
/**
 * @brief The MessageCoreSettings class
 */
class MESSAGECORE_EXPORT MessageCoreSettings : public MessageCore::MessageCoreSettingsBase
{
    Q_OBJECT

public:
    static MessageCoreSettings *self();

private:
    MESSAGECORE_NO_EXPORT MessageCoreSettings();
    ~MessageCoreSettings() override;
    static MessageCoreSettings *mSelf;
};
}
