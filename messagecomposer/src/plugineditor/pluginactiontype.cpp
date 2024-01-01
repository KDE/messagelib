/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "pluginactiontype.h"

using namespace MessageComposer;

PluginActionType::PluginActionType() = default;

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
    case MessageComposer::PluginActionType::Insert:
        return QStringLiteral("_plugins_insert");
    case MessageComposer::PluginActionType::View:
        return QStringLiteral("_plugins_view");
    case MessageComposer::PluginActionType::None:
        return {};
    }
    return {};
}
