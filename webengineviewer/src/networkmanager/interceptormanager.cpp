/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

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
#include <qtwebenginewidgetsversion.h>

using namespace WebEngineViewer;

class WebEngineViewer::NetworkAccessManagerWebEnginePrivate
{
public:
    NetworkAccessManagerWebEnginePrivate()
    {
    }

    WebEngineViewer::NetworkUrlInterceptorManager *mManager = nullptr;
    WebEngineViewer::NetworkUrlInterceptor *mNetworkUrlInterceptor = nullptr;
};

InterceptorManager::InterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent)
    : QObject(parent)
    , d(new NetworkAccessManagerWebEnginePrivate)
{
    d->mManager = new WebEngineViewer::NetworkUrlInterceptorManager(webEngine, ac, this);

    // Add interceptor.
    d->mNetworkUrlInterceptor = new WebEngineViewer::NetworkUrlInterceptor(this);
    const auto interfaceList = d->mManager->interfaceList();
    for (WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface : interfaceList) {
        d->mNetworkUrlInterceptor->addInterceptor(interface);
    }
    webEngine->page()->profile()->setUrlRequestInterceptor(d->mNetworkUrlInterceptor);
}

InterceptorManager::~InterceptorManager()
{
    delete d;
}

void InterceptorManager::addInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor)
{
    d->mNetworkUrlInterceptor->addInterceptor(interceptor);
}

void InterceptorManager::removeInterceptor(WebEngineViewer::NetworkPluginUrlInterceptorInterface *interceptor)
{
    d->mNetworkUrlInterceptor->removeInterceptor(interceptor);
}

QList<QAction *> InterceptorManager::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    QList<QAction *> actions;
    const auto interfaceList = d->mManager->interfaceList();
    actions.reserve(interfaceList.count());
    for (WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface : interfaceList) {
        actions.append(interface->interceptorUrlActions(result));
    }
    return actions;
}
