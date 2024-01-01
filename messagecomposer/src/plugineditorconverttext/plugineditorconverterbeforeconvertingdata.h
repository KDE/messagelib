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
 * @brief The PluginEditorConverterBeforeConvertingData class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConverterBeforeConvertingData
{
public:
    PluginEditorConverterBeforeConvertingData();

    [[nodiscard]] bool newMessage() const;
    void setNewMessage(bool newMessage);

    [[nodiscard]] KMime::Message::Ptr message() const;
    void setMessage(const KMime::Message::Ptr &message);

private:
    KMime::Message::Ptr mMessage;
    bool mNewMessage = true;
};
}
