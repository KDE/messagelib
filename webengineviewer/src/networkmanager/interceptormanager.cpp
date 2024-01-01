/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "interceptormanager.h"
#include "urlinterceptor/networkurlinterceptor.h"
#include "urlinterceptor/networkurlinterceptormanager.h"
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>

using namespace WebEngineViewer;

class WebEngineViewer::NetworkAccessManagerWebEnginePrivate
{
public:
    NetworkAccessManagerWebEnginePrivate() = default;

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

InterceptorManager::~InterceptorManager() = default;

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

#include "moc_interceptormanager.cpp"
