/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "networkpluginurlinterceptorinterface.h"
#include "networkurlinterceptormanager.h"
#include "networkurlinterceptorpluginmanager.h"

#include <MessageViewer/WebHitTestResult>

using namespace MessageViewer;

class MessageViewer::NetworkUrlInterceptorManagerPrivate
{
public:
    NetworkUrlInterceptorManagerPrivate(QWebEngineView *webEngine, KActionCollection *ac, NetworkUrlInterceptorManager *qq)
        : q(qq)
    {
        createInterfaces(webEngine, ac);
    }

    QList<QAction *> interceptorUrlActions(const MessageViewer::WebHitTestResult &result) const;
    QVector<NetworkPluginUrlInterceptorConfigureWidgetSetting> configureInterceptorList(QWidget *parent) const;
    void createInterfaces(QWebEngineView *webEngine, KActionCollection *ac);
    QVector<MessageViewer::NetworkPluginUrlInterceptorInterface *> mListInterface;
    NetworkUrlInterceptorManager *q;
};

QList<QAction *> NetworkUrlInterceptorManagerPrivate::interceptorUrlActions(const MessageViewer::WebHitTestResult &result) const
{
    QList<QAction *> lstActions;
    Q_FOREACH(MessageViewer::NetworkPluginUrlInterceptorInterface *interface, mListInterface) {
        lstActions.append(interface->interceptorUrlActions(result));
    }
    return lstActions;
}

QVector<NetworkPluginUrlInterceptorConfigureWidgetSetting> NetworkUrlInterceptorManagerPrivate::configureInterceptorList(QWidget *parent) const
{
    QVector<NetworkPluginUrlInterceptorConfigureWidgetSetting> lstConfigureWidget;
    Q_FOREACH (NetworkPluginUrlInterceptor *plugin, NetworkUrlInterceptorPluginManager::self()->pluginsList()) {
        if (plugin->hasConfigureSupport()) {
            lstConfigureWidget.append(plugin->createConfigureWidget(parent));
        }
    }
    return lstConfigureWidget;
}

void NetworkUrlInterceptorManagerPrivate::createInterfaces(QWebEngineView *webEngine, KActionCollection *ac)
{
    Q_FOREACH (NetworkPluginUrlInterceptor *plugin, NetworkUrlInterceptorPluginManager::self()->pluginsList()) {
        MessageViewer::NetworkPluginUrlInterceptorInterface *interface = plugin->createInterface(webEngine, q);
        interface->createActions(ac);
        mListInterface.append(interface);
    }
}

NetworkUrlInterceptorManager::NetworkUrlInterceptorManager(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent)
    : QObject(parent),
      d(new NetworkUrlInterceptorManagerPrivate(webEngine, ac, this))
{

}

NetworkUrlInterceptorManager::~NetworkUrlInterceptorManager()
{
    delete d;
}

QVector<MessageViewer::NetworkPluginUrlInterceptorInterface *> NetworkUrlInterceptorManager::interfaceList() const
{
    return d->mListInterface;
}

QList<QAction *> NetworkUrlInterceptorManager::interceptorUrlActions(const MessageViewer::WebHitTestResult &result) const
{
    return d->interceptorUrlActions(result);
}

QVector<NetworkPluginUrlInterceptorConfigureWidgetSetting> NetworkUrlInterceptorManager::configureInterceptorList(QWidget *parent) const
{
    return d->configureInterceptorList(parent);
}

