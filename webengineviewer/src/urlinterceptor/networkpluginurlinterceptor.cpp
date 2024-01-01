/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "networkpluginurlinterceptor.h"

using namespace WebEngineViewer;

NetworkPluginUrlInterceptor::NetworkPluginUrlInterceptor(QObject *parent)
    : QObject(parent)
{
}

NetworkPluginUrlInterceptor::~NetworkPluginUrlInterceptor() = default;

bool NetworkPluginUrlInterceptor::hasConfigureDialog() const
{
    return false;
}

void NetworkPluginUrlInterceptor::showConfigureDialog(QWidget *parent)
{
    Q_UNUSED(parent)
}

void NetworkPluginUrlInterceptor::setIsEnabled(bool enabled)
{
    mIsEnabled = enabled;
}

bool NetworkPluginUrlInterceptor::isEnabled() const
{
    return mIsEnabled;
}

#include "moc_networkpluginurlinterceptor.cpp"
