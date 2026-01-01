/*
   SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "pluginactiontype.h"
using namespace Qt::Literals::StringLiterals;

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
        return u"_plugins_tools"_s;
    case MessageComposer::PluginActionType::Edit:
        return u"_plugins_edit"_s;
    case MessageComposer::PluginActionType::File:
        return u"_plugins_file"_s;
    case MessageComposer::PluginActionType::Action:
        return u"_plugins_actions"_s;
    case MessageComposer::PluginActionType::PopupMenu:
        return u"_popupmenu_actions"_s;
    case MessageComposer::PluginActionType::ToolBar:
        return u"_toolbar_actions"_s;
    case MessageComposer::PluginActionType::Options:
        return u"_plugins_options"_s;
    case MessageComposer::PluginActionType::Insert:
        return u"_plugins_insert"_s;
    case MessageComposer::PluginActionType::View:
        return u"_plugins_view"_s;
    case MessageComposer::PluginActionType::None:
        return {};
    }
    return {};
}
