/*
   Copyright (C) 2018 Laurent Montel <montel@kde.org>

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

#include "plugineditorconverttextinterface.h"

#include <KActionCollection>

using namespace MessageComposer;

class MessageComposer::PluginEditorConvertTextInterfacePrivate
{
public:
    PluginEditorConvertTextInterfacePrivate()
        : mParentWidget(nullptr)
        , mEditor(nullptr)
    {
    }
    PluginActionType mActionType;

    QWidget *mParentWidget = nullptr;
    KPIMTextEdit::RichTextComposer *mEditor = nullptr;
};

PluginEditorConvertTextInterface::PluginEditorConvertTextInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorConvertTextInterfacePrivate)
{
}

PluginEditorConvertTextInterface::~PluginEditorConvertTextInterface()
{
    delete d;
}

bool PluginEditorConvertTextInterface::reformatText()
{
    return false;
}

void PluginEditorConvertTextInterface::setParentWidget(QWidget *parent)
{
    d->mParentWidget = parent;
}

QWidget *PluginEditorConvertTextInterface::parentWidget() const
{
    return d->mParentWidget;
}

void PluginEditorConvertTextInterface::setActionType(PluginActionType type)
{
    d->mActionType = type;
}

PluginActionType PluginEditorConvertTextInterface::actionType() const
{
    return d->mActionType;
}

void PluginEditorConvertTextInterface::createAction(KActionCollection *ac)
{
    Q_UNUSED(ac);
}

KPIMTextEdit::RichTextComposer *PluginEditorConvertTextInterface::richTextEditor() const
{
    return d->mEditor;
}

void PluginEditorConvertTextInterface::setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor)
{
    d->mEditor = richTextEditor;
}

void PluginEditorConvertTextInterface::reloadConfig()
{
    //Reimplement it
}
