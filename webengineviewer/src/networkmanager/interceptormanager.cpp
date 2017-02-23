/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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
#include "interceptormanager.h"
#include "urlinterceptor/networkurlinterceptormanager.h"
#include "urlinterceptor/networkurlinterceptor.h"
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>

using namespace WebEngineViewer;

class WebEngineViewer::NetworkAccessManagerWebEnginePrivate
{
public:
    NetworkAccessManagerWebEnginePrivate()
        : mManager(nullptr),
          mNetworkUrlInterceptor(nullptr)
    {

    }
    WebEngineViewer::NetworkUrlInterceptorManager *mManager;
    WebEngineViewer::NetworkUrlInterceptor *mNetworkUrlInterceptor;
};

InterceptorManager::InterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent)
    : QObject(parent),
      d(new NetworkAccessManagerWebEnginePrivate)
{
    d->mManager = new WebEngineViewer::NetworkUrlInterceptorManager(webEngine, ac, this);

    // Add interceptor.
    d->mNetworkUrlInterceptor = new WebEngineViewer::NetworkUrlInterceptor(this);
    for (WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface : d->mManager->interfaceList()) {
        d->mNetworkUrlInterceptor->addInterceptor(interface);
    }
    webEngine->page()->profile()->setRequestInterceptor(d->mNetworkUrlInterceptor);
}

InterceptorManager::~InterceptorManager()
{
    delete d;
}

void InterceptorManager::addInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor)
{
    d->mNetworkUrlInterceptor->addInterceptor(interceptor);
}

QList<QAction *> InterceptorManager::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    QList<QAction *> actions;
    for (WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface : d->mManager->interfaceList()) {
        actions.append(interface->interceptorUrlActions(result));
    }
    return actions;
}
