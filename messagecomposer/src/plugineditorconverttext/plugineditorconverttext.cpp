/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorconverttext.h"

using namespace MessageComposer;

PluginEditorConvertText::PluginEditorConvertText(QObject *parent)
    : PluginEditorBase(parent)
{
}

PluginEditorConvertText::~PluginEditorConvertText()
{
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
