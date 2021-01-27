/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorcheckbeforesendinterface.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorCheckBeforeSendInterfacePrivate
{
public:
    PluginEditorCheckBeforeSendInterfacePrivate()
    {
    }

    MessageComposer::PluginEditorCheckBeforeSendParams parameters;
    QWidget *mParentWidget = nullptr;
};

PluginEditorCheckBeforeSendInterface::PluginEditorCheckBeforeSendInterface(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorCheckBeforeSendInterfacePrivate)
{
}

PluginEditorCheckBeforeSendInterface::~PluginEditorCheckBeforeSendInterface()
{
    delete d;
}

void PluginEditorCheckBeforeSendInterface::setParentWidget(QWidget *parent)
{
    d->mParentWidget = parent;
}

QWidget *PluginEditorCheckBeforeSendInterface::parentWidget() const
{
    return d->mParentWidget;
}

void PluginEditorCheckBeforeSendInterface::setParameters(const MessageComposer::PluginEditorCheckBeforeSendParams &params)
{
    d->parameters = params;
}

MessageComposer::PluginEditorCheckBeforeSendParams PluginEditorCheckBeforeSendInterface::parameters() const
{
    return d->parameters;
}

void PluginEditorCheckBeforeSendInterface::reloadConfig()
{
    // Reimplement it
}
