/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <KMime/Message>
namespace MessageComposer
{
/**
 * @brief The PluginEditorConverterInitialData class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConverterInitialData
{
public:
    PluginEditorConverterInitialData();

    [[nodiscard]] bool newMessage() const;
    void setNewMessage(bool newMessage);

    [[nodiscard]] KMime::Message::Ptr mewMsg() const;
    void setMewMsg(const KMime::Message::Ptr &mewMsg);

private:
    KMime::Message::Ptr mMewMsg;
    bool mNewMessage = true;
};
}
