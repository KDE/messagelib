/*
   Copyright (c) 2015-2017 Laurent Montel <montel@kde.org>

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

#include "plugineditorinterface.h"

using namespace MessageComposer;

ActionType::ActionType()
    : mAction(nullptr)
    , mType(Tools)
{
}

ActionType::ActionType(QAction *action, ActionType::Type type)
    : mAction(action)
    , mType(type)
{
}

QAction *ActionType::action() const
{
    return mAction;
}

ActionType::Type ActionType::type() const
{
    return mType;
}

class MessageComposer::PluginEditorInterfacePrivate
{
public:
    PluginEditorInterfacePrivate()
    {
    }

    ActionType mActionType;
    QWidget *mParentWidget = nullptr;
    KPIMTextEdit::RichTextEditor *mRichTextEditor = nullptr;
    PluginEditor *plugin = nullptr;
};

PluginEditorInterface::PluginEditorInterface(QObject *parent)
    : PimCommon::AbstractGenericPluginInterface(parent)
    , d(new MessageComposer::PluginEditorInterfacePrivate)
{
}

PluginEditorInterface::~PluginEditorInterface()
{
    delete d;
}

void PluginEditorInterface::setActionType(ActionType type)
{
    d->mActionType = type;
}

ActionType PluginEditorInterface::actionType() const
{
    return d->mActionType;
}

KPIMTextEdit::RichTextEditor *PluginEditorInterface::richTextEditor() const
{
    return d->mRichTextEditor;
}

void PluginEditorInterface::setRichTextEditor(KPIMTextEdit::RichTextEditor *richTextEditor)
{
    d->mRichTextEditor = richTextEditor;
}

QString PluginEditorInterface::actionXmlExtension(ActionType::Type type)
{
    switch (type) {
    case MessageComposer::ActionType::Tools:
        return QStringLiteral("_plugins_tools");
    case MessageComposer::ActionType::Edit:
        return QStringLiteral("_plugins_edit");
    case MessageComposer::ActionType::File:
        return QStringLiteral("_plugins_file");
    case MessageComposer::ActionType::Action:
        return QStringLiteral("_plugins_actions");
    case MessageComposer::ActionType::PopupMenu:
        return QStringLiteral("_popupmenu_actions");
    case MessageComposer::ActionType::ToolBar:
        return QStringLiteral("_toolbar_actions");
    case MessageComposer::ActionType::Options:
        return QStringLiteral("_plugins_options");
    case MessageComposer::ActionType::None:
        return QString();
    }
    return {};
}
