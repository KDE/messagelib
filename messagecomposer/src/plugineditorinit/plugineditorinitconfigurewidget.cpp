/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorinitconfigurewidget.h"

using namespace MessageComposer;

PluginEditorInitConfigureWidget::PluginEditorInitConfigureWidget(QWidget *parent)
    : QWidget(parent)
{
}

PluginEditorInitConfigureWidget::~PluginEditorInitConfigureWidget()
{
}

QString PluginEditorInitConfigureWidget::helpAnchor() const
{
    return QString();
}
