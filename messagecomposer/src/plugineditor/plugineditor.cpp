/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditor.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorPrivate
{
public:
    PluginEditorPrivate() = default;

    int order = 0;
};

PluginEditor::PluginEditor(QObject *parent)
    : PimCommon::AbstractGenericPlugin(parent)
    , d(new PluginEditorPrivate)
{
}

PluginEditor::~PluginEditor() = default;

void PluginEditor::setOrder(int order)
{
    d->order = order;
}

int PluginEditor::order() const
{
    return d->order;
}

bool PluginEditor::canProcessKeyEvent() const
{
    return false;
}

#include "moc_plugineditor.cpp"
