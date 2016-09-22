/*
   Copyright (C) 2015-2016 Laurent Montel <montel@kde.org>

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
#include "messagecomposer_export.h"
class QAction;
class KActionCollection;

namespace KPIMTextEdit
{
class RichTextEditor;
}

namespace MessageComposer
{
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
        Options = 6
    };
    ActionType();

    ActionType(QAction *action, Type type);
    QAction *action() const;
    Type type() const;

private:
    QAction *mAction;
    Type mType;
};

class PluginEditorInterfacePrivate;
class PluginEditor;
class MESSAGECOMPOSER_EXPORT PluginEditorInterface : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorInterface(QObject *parent = Q_NULLPTR);
    ~PluginEditorInterface();

    void setActionType(const ActionType &type);
    ActionType actionType() const;

    virtual void createAction(KActionCollection *ac) = 0;
    virtual void exec() = 0;

    void setPlugin(PluginEditor *plugin);
    PluginEditor *plugin() const;

    void setParentWidget(QWidget *parent);
    QWidget *parentWidget() const;

    KPIMTextEdit::RichTextEditor *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextEditor *richTextEditor);

    virtual bool hasPopupMenuSupport() const;
    virtual bool hasConfigureDialog() const;
    virtual bool hasToolBarSupport() const;
    virtual void showConfigureDialog(QWidget *parentWidget = Q_NULLPTR);

    static QString actionXmlExtension(ActionType::Type type);
Q_SIGNALS:
    void emitPluginActivated(MessageComposer::PluginEditorInterface *interface);

private:
    PluginEditorInterfacePrivate *const d;
};
}

#endif // PLUGINEDITORINTERFACE_H
