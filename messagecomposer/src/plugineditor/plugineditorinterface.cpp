/*
   SPDX-FileCopyrightText: 2015-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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
