/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] std::shared_ptr<KMime::Message> mewMsg() const;
    void setMewMsg(const std::shared_ptr<KMime::Message> &mewMsg);

private:
    std::shared_ptr<KMime::Message> mMewMsg;
    bool mNewMessage = true;
};
}
