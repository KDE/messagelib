/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorinit.h"

using namespace MessageComposer;

PluginEditorInit::PluginEditorInit(QObject *parent)
    : PluginEditorBase(parent)
{
}

PluginEditorInit::~PluginEditorInit() = default;

#include "moc_plugineditorinit.cpp"
