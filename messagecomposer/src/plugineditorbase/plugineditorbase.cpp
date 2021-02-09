/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorbase.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorBasePrivate
{
public:
    PluginEditorBasePrivate()
    {
    }

    bool mIsEnabled = false;
};

PluginEditorBase::PluginEditorBase(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorBasePrivate)
{
}

PluginEditorBase::~PluginEditorBase()
{
    delete d;
}

bool PluginEditorBase::hasConfigureDialog() const
{
    return false;
}

void PluginEditorBase::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent)
}

void PluginEditorBase::emitConfigChanged()
{
    Q_EMIT configChanged();
}

QString PluginEditorBase::description() const
{
    return {};
}

void PluginEditorBase::setIsEnabled(bool enabled)
{
    d->mIsEnabled = enabled;
}

bool PluginEditorBase::isEnabled() const
{
    return d->mIsEnabled;
}
