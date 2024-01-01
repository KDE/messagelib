/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorinterface.h"
#include "plugincomposerinterface.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorInterfacePrivate
{
public:
    PluginEditorInterfacePrivate() = default;

    ~PluginEditorInterfacePrivate()
    {
        delete mComposerInterface;
    }

    PluginEditorInterface::ApplyOnFieldTypes mApplyOnFieldTypes = {PluginEditorInterface::ApplyOnFieldType::All};
    PluginActionType mActionType;
    QWidget *mParentWidget = nullptr;
    TextCustomEditor::RichTextEditor *mRichTextEditor = nullptr;
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

PluginEditorInterface::~PluginEditorInterface() = default;

void PluginEditorInterface::setActionType(PluginActionType type)
{
    d->mActionType = type;
}

PluginActionType PluginEditorInterface::actionType() const
{
    return d->mActionType;
}

TextCustomEditor::RichTextEditor *PluginEditorInterface::richTextEditor() const
{
    return d->mRichTextEditor;
}

void PluginEditorInterface::setRichTextEditor(TextCustomEditor::RichTextEditor *richTextEditor)
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
    Q_UNUSED(event)
    return false;
}

PluginEditorInterface::ApplyOnFieldTypes PluginEditorInterface::applyOnFieldTypes() const
{
    return d->mApplyOnFieldTypes;
}

void PluginEditorInterface::setApplyOnFieldTypes(PluginEditorInterface::ApplyOnFieldTypes types)
{
    d->mApplyOnFieldTypes = types;
}

#include "moc_plugineditorinterface.cpp"
