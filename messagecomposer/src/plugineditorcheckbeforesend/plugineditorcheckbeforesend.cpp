/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorcheckbeforesend.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorCheckBeforeSendPrivate
{
public:
    PluginEditorCheckBeforeSendPrivate()
    {
    }

    bool mIsEnabled = false;
};

PluginEditorCheckBeforeSend::PluginEditorCheckBeforeSend(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorCheckBeforeSendPrivate)
{
}

PluginEditorCheckBeforeSend::~PluginEditorCheckBeforeSend()
{
    delete d;
}

bool PluginEditorCheckBeforeSend::hasConfigureDialog() const
{
    return false;
}

void PluginEditorCheckBeforeSend::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent);
}

void PluginEditorCheckBeforeSend::emitConfigChanged()
{
    Q_EMIT configChanged();
}

QString PluginEditorCheckBeforeSend::description() const
{
    return {};
}

void PluginEditorCheckBeforeSend::setIsEnabled(bool enabled)
{
    d->mIsEnabled = enabled;
}

bool PluginEditorCheckBeforeSend::isEnabled() const
{
    return d->mIsEnabled;
}
