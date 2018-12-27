/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

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
    Q_UNUSED(parent);
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
