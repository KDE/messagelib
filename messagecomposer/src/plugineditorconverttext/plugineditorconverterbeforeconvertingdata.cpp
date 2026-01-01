/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

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

std::shared_ptr<KMime::Message> PluginEditorConverterBeforeConvertingData::message() const
{
    return mMessage;
}

void PluginEditorConverterBeforeConvertingData::setMessage(const std::shared_ptr<KMime::Message> &message)
{
    mMessage = message;
}
