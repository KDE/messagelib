/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#include "plugineditorconverttextinterface.h"
#include "plugineditorconverterinitialdata.h"
#include "plugineditorconverterbeforeconvertingdata.h"

#include <KActionCollection>

using namespace MessageComposer;

class MessageComposer::PluginEditorConvertTextInterfacePrivate
{
public:
    PluginEditorConvertTextInterfacePrivate()
    {
    }

    QVector<PluginActionType> mActionTypes;

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

PluginEditorConvertTextInterface::~PluginEditorConvertTextInterface()
{
    delete d;
}

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

void PluginEditorConvertTextInterface::setActionType(const QVector<PluginActionType> &type)
{
    d->mActionTypes = type;
}

QVector<PluginActionType> PluginEditorConvertTextInterface::actionTypes() const
{
    return d->mActionTypes;
}

void PluginEditorConvertTextInterface::addActionType(const PluginActionType &type)
{
    d->mActionTypes += type;
}

void PluginEditorConvertTextInterface::createAction(KActionCollection *ac)
{
    Q_UNUSED(ac);
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
    //Reimplement it
}

void PluginEditorConvertTextInterface::enableDisablePluginActions(bool richText)
{
    Q_UNUSED(richText);
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
