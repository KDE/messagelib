/*
   Copyright (C) 2017-2018 Laurent Montel <montel@kde.org>

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

#include "plugineditorconverttext.h"

using namespace MessageComposer;

class MessageComposer::PluginEditorConverttextPrivate
{
public:
    PluginEditorConverttextPrivate()
    {
    }

    bool mIsEnabled = false;
};

PluginEditorConvertText::PluginEditorConvertText(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorConverttextPrivate)
{
}

PluginEditorConvertText::~PluginEditorConvertText()
{
    delete d;
}

bool PluginEditorConvertText::hasConfigureDialog() const
{
    return false;
}

void PluginEditorConvertText::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent);
}

void PluginEditorConvertText::emitConfigChanged()
{
    Q_EMIT configChanged();
}

QString PluginEditorConvertText::description() const
{
    return {};
}

void PluginEditorConvertText::setIsEnabled(bool enabled)
{
    d->mIsEnabled = enabled;
}

bool PluginEditorConvertText::isEnabled() const
{
    return d->mIsEnabled;
}

bool PluginEditorConvertText::canWorkOnHtml() const
{
    return true;
}
