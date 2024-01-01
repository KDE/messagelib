/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorconfigurebasewidget.h"

using namespace MessageComposer;

PluginEditorConfigureBaseWidget::PluginEditorConfigureBaseWidget(QWidget *parent)
    : QWidget(parent)
{
}

PluginEditorConfigureBaseWidget::~PluginEditorConfigureBaseWidget() = default;

QString PluginEditorConfigureBaseWidget::helpAnchor() const
{
    return {};
}

#include "moc_plugineditorconfigurebasewidget.cpp"
