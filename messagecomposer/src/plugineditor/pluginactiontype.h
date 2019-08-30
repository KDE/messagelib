/*
   Copyright (C) 2015-2019 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINACTIONTYPE_H
#define PLUGINACTIONTYPE_H

#include "messagecomposer_export.h"
#include <QString>
class QAction;

namespace MessageComposer {
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
        None = 7
    };
    PluginActionType();

    PluginActionType(QAction *action, Type type);
    Q_REQUIRED_RESULT QAction *action() const;
    Q_REQUIRED_RESULT Type type() const;

    static QString actionXmlExtension(PluginActionType::Type type);
private:
    QAction *mAction = nullptr;
    Type mType = Tools;
};
}
Q_DECLARE_TYPEINFO(MessageComposer::PluginActionType, Q_MOVABLE_TYPE);

#endif
