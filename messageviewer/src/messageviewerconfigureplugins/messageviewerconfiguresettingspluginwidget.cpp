/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewerconfiguresettingspluginwidget.h"

using namespace MessageViewer;

MessageViewerConfigureSettingsPluginWidget::MessageViewerConfigureSettingsPluginWidget(QWidget *parent)
    : QWidget(parent)
{
}

MessageViewerConfigureSettingsPluginWidget::~MessageViewerConfigureSettingsPluginWidget()
{
}

QString MessageViewerConfigureSettingsPluginWidget::helpAnchor() const
{
    return QString();
}
