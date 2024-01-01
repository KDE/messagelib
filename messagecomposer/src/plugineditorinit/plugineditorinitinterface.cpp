/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorinitinterface.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorInitInterfacePrivate
{
public:
    PluginEditorInitInterfacePrivate() = default;

    QWidget *mParentWidget = nullptr;
    KPIMTextEdit::RichTextComposer *mEditor = nullptr;
};

PluginEditorInitInterface::PluginEditorInitInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorInitInterfacePrivate)
{
}

PluginEditorInitInterface::~PluginEditorInitInterface() = default;

void PluginEditorInitInterface::setParentWidget(QWidget *parent)
{
    d->mParentWidget = parent;
}

QWidget *PluginEditorInitInterface::parentWidget() const
{
    return d->mParentWidget;
}

KPIMTextEdit::RichTextComposer *PluginEditorInitInterface::richTextEditor() const
{
    return d->mEditor;
}

void PluginEditorInitInterface::setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor)
{
    d->mEditor = richTextEditor;
}

void PluginEditorInitInterface::reloadConfig()
{
    // Reimplement it
}

#include "moc_plugineditorinitinterface.cpp"
