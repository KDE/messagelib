/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorconverterbeforeconvertingdata.h"
using namespace MessageComposer;
PluginEditorConverterBeforeConvertingData::PluginEditorConverterBeforeConvertingData() = default;

bool PluginEditorConverterBeforeConvertingData::newMessage() const
{
    return mNewMessage;
}

void PluginEditorConverterBeforeConvertingData::setNewMessage(bool newMessage)
{
    mNewMessage = newMessage;
}

KMime::Message::Ptr PluginEditorConverterBeforeConvertingData::message() const
{
    return mMessage;
}

void PluginEditorConverterBeforeConvertingData::setMessage(const KMime::Message::Ptr &message)
{
    mMessage = message;
}
