/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    //Reimplement it
}
