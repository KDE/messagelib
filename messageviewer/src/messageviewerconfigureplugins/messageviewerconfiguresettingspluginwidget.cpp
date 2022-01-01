/*
   SPDX-FileCopyrightText: 2018-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewerconfiguresettingspluginwidget.h"

using namespace MessageViewer;

MessageViewerConfigureSettingsPluginWidget::MessageViewerConfigureSettingsPluginWidget(QWidget *parent)
    : QWidget(parent)
{
}

MessageViewerConfigureSettingsPluginWidget::~MessageViewerConfigureSettingsPluginWidget() = default;

QString MessageViewerConfigureSettingsPluginWidget::helpAnchor() const
{
    return {};
}
