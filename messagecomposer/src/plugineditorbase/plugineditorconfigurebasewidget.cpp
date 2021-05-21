/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorconfigurebasewidget.h"

using namespace MessageComposer;

PluginEditorConfigureBaseWidget::PluginEditorConfigureBaseWidget(QWidget *parent)
    : QWidget(parent)
{
}

PluginEditorConfigureBaseWidget::~PluginEditorConfigureBaseWidget()
{
}

QString PluginEditorConfigureBaseWidget::helpAnchor() const
{
    return QString();
}
