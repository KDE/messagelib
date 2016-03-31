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

#include "networkaccessmanagerwebengine.h"
#include "webengine/urlinterceptor/networkurlinterceptormanager.h"
#include "webengine/urlinterceptor/networkurlinterceptor.h"
#include <QWebEngineProfile>
#include <MessageViewer/NetworkPluginUrlInterceptorInterface>

using namespace MessageViewer;

class MessageViewer::NetworkAccessManagerWebEnginePrivate
{
public:
    NetworkAccessManagerWebEnginePrivate()
        : mManager(Q_NULLPTR),
          mNetworkUrlInterceptor(Q_NULLPTR)
    {

    }
    MessageViewer::NetworkUrlInterceptorManager *mManager;
    MessageViewer::NetworkUrlInterceptor *mNetworkUrlInterceptor;
};

NetworkAccessManagerWebEngine::NetworkAccessManagerWebEngine(QWebEngineView *webEngine, KActionCollection *ac, QObject *parent)
    : QNetworkAccessManager(parent),
      d(new NetworkAccessManagerWebEnginePrivate)
{
    d->mManager = new MessageViewer::NetworkUrlInterceptorManager(webEngine, ac, this);

    // Add interceptor.
    d->mNetworkUrlInterceptor = new MessageViewer::NetworkUrlInterceptor(this);
    Q_FOREACH (MessageViewer::NetworkPluginUrlInterceptorInterface *interface, d->mManager->interfaceList()) {
        d->mNetworkUrlInterceptor->addInterceptor(interface);
    }
    QWebEngineProfile::defaultProfile()->setRequestInterceptor(d->mNetworkUrlInterceptor);
}

NetworkAccessManagerWebEngine::~NetworkAccessManagerWebEngine()
{
    delete d;
}

void NetworkAccessManagerWebEngine::addInterceptor(MessageViewer::NetworkPluginUrlInterceptorInterface *interceptor)
{
    d->mNetworkUrlInterceptor->addInterceptor(interceptor);
}

QList<QAction *> NetworkAccessManagerWebEngine::actions() const
{
    QList<QAction *> actions;
    Q_FOREACH (MessageViewer::NetworkPluginUrlInterceptorInterface *interface, d->mManager->interfaceList()) {
        actions.append(interface->actions());
    }
    return actions;
}
