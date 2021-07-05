/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "networkurlinterceptor.h"
#include "networkpluginurlinterceptorinterface.h"
#include "webengineviewer_debug.h"

#include <QVector>

using namespace WebEngineViewer;

class WebEngineViewer::NetworkUrlInterceptorPrivate
{
public:
    NetworkUrlInterceptorPrivate()
    {
    }

    void addInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);
    void interceptRequest(QWebEngineUrlRequestInfo &info);
    void removeInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);
    QVector<NetworkPluginUrlInterceptorInterface *> listInterceptor;
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

NetworkUrlInterceptor::~NetworkUrlInterceptor()
{
    delete d;
}

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
