/*
  Copyright (c) 2015-2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "plugineditorinterface.h"

using namespace MessageComposer;

ActionType::ActionType()
    : mAction(Q_NULLPTR),
      mType(Tools)
{

}

ActionType::ActionType(QAction *action, ActionType::Type type)
    : mAction(action),
      mType(type)
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
        : mParentWidget(Q_NULLPTR),
          mRichTextEditor(Q_NULLPTR)
    {

    }
    ActionType mActionType;
    QWidget *mParentWidget;
    KPIMTextEdit::RichTextEditor *mRichTextEditor;
};

PluginEditorInterface::PluginEditorInterface(QObject *parent)
    : QObject(parent),
      d(new MessageComposer::PluginEditorInterfacePrivate)
{

}

PluginEditorInterface::~PluginEditorInterface()
{
    delete d;
}

void PluginEditorInterface::setActionType(const ActionType &type)
{
    d->mActionType = type;
}

ActionType PluginEditorInterface::actionType() const
{
    return d->mActionType;
}

void PluginEditorInterface::setParentWidget(QWidget *parent)
{
    d->mParentWidget = parent;
}

QWidget *PluginEditorInterface::parentWidget() const
{
    return d->mParentWidget;
}

KPIMTextEdit::RichTextEditor *PluginEditorInterface::richTextEditor() const
{
    return d->mRichTextEditor;
}

void PluginEditorInterface::setRichTextEditor(KPIMTextEdit::RichTextEditor *richTextEditor)
{
    d->mRichTextEditor = richTextEditor;
}

bool PluginEditorInterface::hasPopupMenuSupport() const
{
    return false;
}

bool PluginEditorInterface::hasConfigureDialog() const
{
    return false;
}

bool MessageComposer::PluginEditorInterface::hasToolBarSupport() const
{
    return false;
}

void PluginEditorInterface::showConfigureDialog(QWidget *parentWidget)
{
    Q_UNUSED(parentWidget);
}

QString PluginEditorInterface::actionXmlExtension(ActionType::Type type)
{
    switch(type) {
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
    }
    return {};
}


