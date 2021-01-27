/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorinitinterface.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorInitInterfacePrivate
{
public:
    PluginEditorInitInterfacePrivate()
    {
    }

    QWidget *mParentWidget = nullptr;
    KPIMTextEdit::RichTextComposer *mEditor = nullptr;
};

PluginEditorInitInterface::PluginEditorInitInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorInitInterfacePrivate)
{
}

PluginEditorInitInterface::~PluginEditorInitInterface()
{
    delete d;
}

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
