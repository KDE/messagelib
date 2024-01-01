/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "networkurlinterceptor.h"
#include "networkpluginurlinterceptorinterface.h"

#include <QDebug>
#include <QList>

using namespace WebEngineViewer;

class WebEngineViewer::NetworkUrlInterceptorPrivate
{
public:
    NetworkUrlInterceptorPrivate() = default;

    void addInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);
    void interceptRequest(QWebEngineUrlRequestInfo &info);
    void removeInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);
    QList<NetworkPluginUrlInterceptorInterface *> listInterceptor;
};

void NetworkUrlInterceptorPrivate::addInterceptor(NetworkPluginUrlInterceptorInterface *interceptor)
{
    if (!listInterceptor.contains(interceptor)) {
        listInterceptor.append(interceptor);
    }
}

void NetworkUrlInterceptorPrivate::removeInterceptor(NetworkPluginUrlInterceptorInterface *interceptor)
{
    if (listInterceptor.contains(interceptor)) {
        listInterceptor.removeOne(interceptor);
    }
}

void NetworkUrlInterceptorPrivate::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    for (NetworkPluginUrlInterceptorInterface *inter : std::as_const(listInterceptor)) {
        if (inter->interceptRequest(info)) {
            // qDebug() << " intercep " << info.requestUrl() << " inter " << inter;
            info.block(true);
            break;
        }
    }
}

NetworkUrlInterceptor::NetworkUrlInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent)
    , d(new NetworkUrlInterceptorPrivate)
{
}

NetworkUrlInterceptor::~NetworkUrlInterceptor() = default;

void NetworkUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    d->interceptRequest(info);
}

void NetworkUrlInterceptor::addInterceptor(NetworkPluginUrlInterceptorInterface *interceptor)
{
    d->addInterceptor(interceptor);
}

void NetworkUrlInterceptor::removeInterceptor(NetworkPluginUrlInterceptorInterface *interceptor)
{
    d->removeInterceptor(interceptor);
}

#include "moc_networkurlinterceptor.cpp"
