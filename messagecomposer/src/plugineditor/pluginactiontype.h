/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QString>
class QAction;

namespace MessageComposer
{
/**
 * @brief The PluginActionType class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginActionType
{
public:
    enum Type {
        Tools = 0,
        Edit = 1,
        File = 2,
        Action = 3,
        PopupMenu = 4,
        ToolBar = 5,
        Options = 6,
        None = 7,
        Insert = 8,
        View = 9,
    };
    PluginActionType();

    explicit PluginActionType(QAction *action, Type type);
    [[nodiscard]] QAction *action() const;
    [[nodiscard]] Type type() const;

    static QString actionXmlExtension(PluginActionType::Type type);

private:
    QAction *mAction = nullptr;
    Type mType = Tools;
};
}
Q_DECLARE_TYPEINFO(MessageComposer::PluginActionType, Q_RELOCATABLE_TYPE);
