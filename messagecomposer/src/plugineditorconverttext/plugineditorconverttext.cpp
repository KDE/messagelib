/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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

bool PluginEditorConvertText::hasToolBarSupport() const
{
    return false;
}

bool PluginEditorConvertText::hasStatusBarSupport() const
{
    return false;
}

bool PluginEditorConvertText::hasPopupMenuSupport() const
{
    return false;
}
