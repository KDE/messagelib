/*
   Copyright (C) 2015-2017 Laurent Montel <montel@kde.org>

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

#ifndef PLUGINEDITORINTERFACE_H
#define PLUGINEDITORINTERFACE_H

#include <QObject>
#include <PimCommon/AbstractGenericPluginInterface>
#include "messagecomposer_export.h"
class QAction;

namespace KPIMTextEdit {
class RichTextEditor;
}

namespace MessageComposer {
class MESSAGECOMPOSER_EXPORT ActionType
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
    ActionType();

    ActionType(QAction *action, Type type);
    QAction *action() const;
    Type type() const;

private:
    QAction *mAction = nullptr;
    Type mType;
};

class PluginEditorInterfacePrivate;
class PluginEditor;
class MESSAGECOMPOSER_EXPORT PluginEditorInterface : public PimCommon::AbstractGenericPluginInterface
{
    Q_OBJECT
public:
    explicit PluginEditorInterface(QObject *parent = nullptr);
    ~PluginEditorInterface();

    void setActionType(ActionType type);
    ActionType actionType() const;

    KPIMTextEdit::RichTextEditor *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextEditor *richTextEditor);

    void setNeedSelectedText(bool b);
    bool needSelectedText() const;

    static QString actionXmlExtension(ActionType::Type type);
Q_SIGNALS:
    void emitPluginActivated(MessageComposer::PluginEditorInterface *interface);

private:
    PluginEditorInterfacePrivate *const d;
};
}

#endif // PLUGINEDITORINTERFACE_H
