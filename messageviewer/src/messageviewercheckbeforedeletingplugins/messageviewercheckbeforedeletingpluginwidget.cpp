/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewercheckbeforedeletingpluginwidget.h"

using namespace MessageViewer;

MessageViewerCheckBeforeDeletingPluginWidget::MessageViewerCheckBeforeDeletingPluginWidget(QWidget *parent)
    : QWidget(parent)
{
}

MessageViewerCheckBeforeDeletingPluginWidget::~MessageViewerCheckBeforeDeletingPluginWidget() = default;

QString MessageViewerCheckBeforeDeletingPluginWidget::helpAnchor() const
{
    return {};
}

#include "moc_messageviewercheckbeforedeletingpluginwidget.cpp"
