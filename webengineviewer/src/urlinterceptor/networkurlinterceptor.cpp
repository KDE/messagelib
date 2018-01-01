/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "networkpluginurlinterceptorinterface.h"
#include "networkurlinterceptor.h"
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
    QVector<NetworkPluginUrlInterceptorInterface *> listInterceptor;
};

void NetworkUrlInterceptorPrivate::addInterceptor(NetworkPluginUrlInterceptorInterface *interceptor)
{
    if (!listInterceptor.contains(interceptor)) {
        listInterceptor.append(interceptor);
    } else {
        qCDebug(WEBENGINEVIEWER_LOG) << "interceptor already added. This is a problem" << interceptor;
    }
}

void NetworkUrlInterceptorPrivate::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    for (NetworkPluginUrlInterceptorInterface *inter : qAsConst(listInterceptor)) {
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
