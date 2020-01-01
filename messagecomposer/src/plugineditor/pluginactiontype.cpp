/*
   Copyright (C) 2015-2020 Laurent Montel <montel@kde.org>

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

#include "pluginactiontype.h"

using namespace MessageComposer;

PluginActionType::PluginActionType()
{
}

PluginActionType::PluginActionType(QAction *action, PluginActionType::Type type)
    : mAction(action)
    , mType(type)
{
}

QAction *PluginActionType::action() const
{
    return mAction;
}

PluginActionType::Type PluginActionType::type() const
{
    return mType;
}

QString PluginActionType::actionXmlExtension(PluginActionType::Type type)
{
    switch (type) {
    case MessageComposer::PluginActionType::Tools:
        return QStringLiteral("_plugins_tools");
    case MessageComposer::PluginActionType::Edit:
        return QStringLiteral("_plugins_edit");
    case MessageComposer::PluginActionType::File:
        return QStringLiteral("_plugins_file");
    case MessageComposer::PluginActionType::Action:
        return QStringLiteral("_plugins_actions");
    case MessageComposer::PluginActionType::PopupMenu:
        return QStringLiteral("_popupmenu_actions");
    case MessageComposer::PluginActionType::ToolBar:
        return QStringLiteral("_toolbar_actions");
    case MessageComposer::PluginActionType::Options:
        return QStringLiteral("_plugins_options");
    case MessageComposer::PluginActionType::None:
        return QString();
    }
    return {};
}
