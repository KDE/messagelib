/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "networkpluginurlinterceptorinterface.h"

using namespace WebEngineViewer;

NetworkPluginUrlInterceptorInterface::NetworkPluginUrlInterceptorInterface(QObject *parent)
    : QObject(parent)
{
}

NetworkPluginUrlInterceptorInterface::~NetworkPluginUrlInterceptorInterface()
{
}

void NetworkPluginUrlInterceptorInterface::createActions(KActionCollection *ac)
{
    Q_UNUSED(ac)
}

QList<QAction *> NetworkPluginUrlInterceptorInterface::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    Q_UNUSED(result)
    return {};
}
