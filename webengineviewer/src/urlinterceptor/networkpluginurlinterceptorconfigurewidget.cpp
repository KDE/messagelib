/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "networkpluginurlinterceptorconfigurewidget.h"

using namespace WebEngineViewer;

NetworkPluginUrlInterceptorConfigureWidget::NetworkPluginUrlInterceptorConfigureWidget(QWidget *parent)
    : QWidget(parent)
{
}

NetworkPluginUrlInterceptorConfigureWidget::~NetworkPluginUrlInterceptorConfigureWidget()
{
}

QString NetworkPluginUrlInterceptorConfigureWidget::helpAnchor() const
{
    return QString();
}
