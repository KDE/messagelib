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

#include "plugincomposerinterface.h"
#include "plugineditorinterface.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorInterfacePrivate
{
public:
    PluginEditorInterfacePrivate()
    {
    }

    ~PluginEditorInterfacePrivate()
    {
        delete mComposerInterface;
    }

    PluginActionType mActionType;
    QWidget *mParentWidget = nullptr;
    KPIMTextEdit::RichTextEditor *mRichTextEditor = nullptr;
    MessageComposer::PluginComposerInterface *mComposerInterface = nullptr;
    PluginEditor *plugin = nullptr;
    QWidget *statusBarWidget = nullptr;
    bool mSelectedText = false;
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

void PluginEditorInterface::setActionType(PluginActionType type)
{
    d->mActionType = type;
}

PluginActionType PluginEditorInterface::actionType() const
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

void PluginEditorInterface::setNeedSelectedText(bool b)
{
    d->mSelectedText = b;
}

bool PluginEditorInterface::needSelectedText() const
{
    return d->mSelectedText;
}

void PluginEditorInterface::setStatusBarWidget(QWidget *w)
{
    d->statusBarWidget = w;
}

QWidget *PluginEditorInterface::statusBarWidget() const
{
    return d->statusBarWidget;
}

PluginComposerInterface *PluginEditorInterface::composerInterface() const
{
    return d->mComposerInterface;
}

void PluginEditorInterface::setComposerInterface(PluginComposerInterface *w)
{
    d->mComposerInterface = w;
}

bool PluginEditorInterface::processProcessKeyEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    return false;
}
