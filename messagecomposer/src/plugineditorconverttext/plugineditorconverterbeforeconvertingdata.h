/*
   SPDX-FileCopyrightText: 2018-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <KMime/Message>
namespace MessageComposer
{
/**
 * @brief The PluginEditorConverterBeforeConvertingData class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConverterBeforeConvertingData
{
public:
    PluginEditorConverterBeforeConvertingData();

    [[nodiscard]] bool newMessage() const;
    void setNewMessage(bool newMessage);

    [[nodiscard]] std::shared_ptr<KMime::Message> message() const;
    void setMessage(const std::shared_ptr<KMime::Message> &message);

private:
    std::shared_ptr<KMime::Message> mMessage;
    bool mNewMessage = true;
};
}
