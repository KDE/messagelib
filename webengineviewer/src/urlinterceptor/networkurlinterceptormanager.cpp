/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "networkurlinterceptormanager.h"
#include "networkurlinterceptorpluginmanager.h"

using namespace WebEngineViewer;

class WebEngineViewer::NetworkUrlInterceptorManagerPrivate
{
public:
    NetworkUrlInterceptorManagerPrivate(QWebEngineView *webEngine, KActionCollection *ac, NetworkUrlInterceptorManager *qq)
        : q(qq)
    {
        createInterfaces(webEngine, ac);
    }

    [[nodiscard]] QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;
    void createInterfaces(QWebEngineView *webEngine, KActionCollection *ac);
    QList<WebEngineViewer::NetworkPluginUrlInterceptorInterface *> mListInterface;

private:
    NetworkUrlInterceptorManager *const q;
};

QList<QAction *> NetworkUrlInterceptorManagerPrivate::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    QList<QAction *> lstActions;
    for (WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface : std::as_const(mListInterface)) {
        lstActions.append(interface->interceptorUrlActions(result));
    }
    return lstActions;
}

void NetworkUrlInterceptorManagerPrivate::createInterfaces(QWebEngineView *webEngine, KActionCollection *ac)
{
    const auto pluginsList = NetworkUrlInterceptorPluginManager::self()->pluginsList();
    for (NetworkPluginUrlInterceptor *plugin : pluginsList) {
        if (plugin->isEnabled()) {
            WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface = plugin->createInterface(webEngine, q);
            interface->createActions(ac);
            mListInterface.append(interface);
        }
    }
}

NetworkUrlInterceptorManager::NetworkUrlInterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent)
    : QObject(parent)
    , d(new NetworkUrlInterceptorManagerPrivate(webEngine, ac, this))
{
}

NetworkUrlInterceptorManager::~NetworkUrlInterceptorManager() = default;

QList<WebEngineViewer::NetworkPluginUrlInterceptorInterface *> NetworkUrlInterceptorManager::interfaceList() const
{
    return d->mListInterface;
}

QList<QAction *> NetworkUrlInterceptorManager::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    return d->interceptorUrlActions(result);
}

#include "moc_networkurlinterceptormanager.cpp"
