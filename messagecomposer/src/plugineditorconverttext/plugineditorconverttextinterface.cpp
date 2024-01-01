/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorconverttextinterface.h"
#include "plugineditorconverterbeforeconvertingdata.h"
#include "plugineditorconverterinitialdata.h"

#include <KActionCollection>

using namespace MessageComposer;

class MessageComposer::PluginEditorConvertTextInterfacePrivate
{
public:
    PluginEditorConvertTextInterfacePrivate() = default;

    QList<PluginActionType> mActionTypes;

    QWidget *mParentWidget = nullptr;
    QWidget *statusBarWidget = nullptr;
    PluginEditorConvertText *plugin = nullptr;
    KPIMTextEdit::RichTextComposer *mEditor = nullptr;
    PluginEditorConverterInitialData mInitialData;
    PluginEditorConverterBeforeConvertingData mBeforeConvertingData;
};

PluginEditorConvertTextInterface::PluginEditorConvertTextInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorConvertTextInterfacePrivate)
{
}

PluginEditorConvertTextInterface::~PluginEditorConvertTextInterface() = default;

bool PluginEditorConvertTextInterface::reformatText()
{
    return false;
}

void PluginEditorConvertTextInterface::setParentWidget(QWidget *parent)
{
    d->mParentWidget = parent;
}

QWidget *PluginEditorConvertTextInterface::parentWidget() const
{
    return d->mParentWidget;
}

void PluginEditorConvertTextInterface::setActionType(const QList<PluginActionType> &type)
{
    d->mActionTypes = type;
}

QList<PluginActionType> PluginEditorConvertTextInterface::actionTypes() const
{
    return d->mActionTypes;
}

void PluginEditorConvertTextInterface::addActionType(PluginActionType type)
{
    d->mActionTypes += type;
}

void PluginEditorConvertTextInterface::createAction(KActionCollection *ac)
{
    Q_UNUSED(ac)
}

void PluginEditorConvertTextInterface::setInitialData(const PluginEditorConverterInitialData &data)
{
    d->mInitialData = data;
}

PluginEditorConverterInitialData PluginEditorConvertTextInterface::initialData() const
{
    return d->mInitialData;
}

void PluginEditorConvertTextInterface::setBeforeConvertingData(const PluginEditorConverterBeforeConvertingData &data)
{
    d->mBeforeConvertingData = data;
}

PluginEditorConverterBeforeConvertingData PluginEditorConvertTextInterface::beforeConvertingData() const
{
    return d->mBeforeConvertingData;
}

KPIMTextEdit::RichTextComposer *PluginEditorConvertTextInterface::richTextEditor() const
{
    return d->mEditor;
}

void PluginEditorConvertTextInterface::setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor)
{
    d->mEditor = richTextEditor;
}

void PluginEditorConvertTextInterface::reloadConfig()
{
    // Reimplement it
}

void PluginEditorConvertTextInterface::enableDisablePluginActions(bool richText)
{
    Q_UNUSED(richText)
}

void PluginEditorConvertTextInterface::setStatusBarWidget(QWidget *w)
{
    d->statusBarWidget = w;
}

QWidget *PluginEditorConvertTextInterface::statusBarWidget() const
{
    return d->statusBarWidget;
}

void PluginEditorConvertTextInterface::setPlugin(PluginEditorConvertText *plugin)
{
    d->plugin = plugin;
}

PluginEditorConvertText *PluginEditorConvertTextInterface::plugin() const
{
    return d->plugin;
}

#include "moc_plugineditorconverttextinterface.cpp"
