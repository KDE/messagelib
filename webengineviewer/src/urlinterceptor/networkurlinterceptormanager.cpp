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

#include "networkpluginurlinterceptorinterface.h"
#include "networkurlinterceptormanager.h"
#include "networkurlinterceptorpluginmanager.h"

#include <WebEngineViewer/WebHitTestResult>

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
    NetworkUrlInterceptorManager *q;
};

QList<QAction *> NetworkUrlInterceptorManagerPrivate::interceptorUrlActions(const WebEngineViewer::WebHitTestResult &result) const
{
    QList<QAction *> lstActions;
    for (WebEngineViewer::NetworkPluginUrlInterceptorInterface *interface : qAsConst(mListInterface)) {
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
