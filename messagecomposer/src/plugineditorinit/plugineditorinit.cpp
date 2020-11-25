/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorinit.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorInitPrivate
{
public:
    PluginEditorInitPrivate()
    {
    }

    bool mIsEnabled = false;
};

PluginEditorInit::PluginEditorInit(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorInitPrivate)
{
}

PluginEditorInit::~PluginEditorInit()
{
    delete d;
}

bool PluginEditorInit::hasConfigureDialog() const
{
    return false;
}

void PluginEditorInit::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent)
}

void PluginEditorInit::emitConfigChanged()
{
    Q_EMIT configChanged();
}

QString PluginEditorInit::description() const
{
    return {};
}

void PluginEditorInit::setIsEnabled(bool enabled)
{
    d->mIsEnabled = enabled;
}

bool PluginEditorInit::isEnabled() const
{
    return d->mIsEnabled;
}
