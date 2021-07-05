/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

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

    Q_REQUIRED_RESULT QList<QAction *> interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const;
    void createInterfaces(QWebEngineView *webEngine, KActionCollection *ac);
    QVector<WebEngineViewer::NetworkPluginUrlInterceptorInterface *> mListInterface;

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
    for (NetworkPluginUrlInterceptor *plugin : NetworkUrlInterceptorPluginManager::self()->pluginsList()) {
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

NetworkUrlInterceptorManager::~NetworkUrlInterceptorManager()
{
    delete d;
}

QVector<WebEngineViewer::NetworkPluginUrlInterceptorInterface *> NetworkUrlInterceptorManager::interfaceList() const
{
    return d->mListInterface;
}

QList<QAction *> NetworkUrlInterceptorManager::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    return d->interceptorUrlActions(result);
}
