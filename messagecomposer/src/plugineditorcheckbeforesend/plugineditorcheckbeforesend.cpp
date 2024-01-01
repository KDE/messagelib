/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorcheckbeforesend.h"

using namespace MessageComposer;

PluginEditorCheckBeforeSend::PluginEditorCheckBeforeSend(QObject *parent)
    : PluginEditorBase(parent)
{
}

PluginEditorCheckBeforeSend::~PluginEditorCheckBeforeSend() = default;

#include "moc_plugineditorcheckbeforesend.cpp"
