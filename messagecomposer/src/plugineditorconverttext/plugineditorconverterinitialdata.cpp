/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorconverterinitialdata.h"

using namespace MessageComposer;

PluginEditorConverterInitialData::PluginEditorConverterInitialData() = default;

bool PluginEditorConverterInitialData::newMessage() const
{
    return mNewMessage;
}

void PluginEditorConverterInitialData::setNewMessage(bool newMessage)
{
    mNewMessage = newMessage;
}

KMime::Message::Ptr PluginEditorConverterInitialData::mewMsg() const
{
    return mMewMsg;
}

void PluginEditorConverterInitialData::setMewMsg(const KMime::Message::Ptr &mewMsg)
{
    mMewMsg = mewMsg;
}
