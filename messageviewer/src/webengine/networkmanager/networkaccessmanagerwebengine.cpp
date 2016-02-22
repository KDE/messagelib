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

using namespace MessageViewer;

class MessageViewer::NetworkAccessManagerWebEnginePrivate
{
public:
    NetworkAccessManagerWebEnginePrivate()
        : mManager(Q_NULLPTR)
    {

    }
    MessageViewer::NetworkUrlInterceptorManager *mManager;
};

NetworkAccessManagerWebEngine::NetworkAccessManagerWebEngine(QObject *parent)
    : QNetworkAccessManager(parent),
      d(new NetworkAccessManagerWebEnginePrivate)
{
    d->mManager = new MessageViewer::NetworkUrlInterceptorManager(this);

    MessageViewer::NetworkUrlInterceptor *networkUrlInterceptor = new MessageViewer::NetworkUrlInterceptor(this);
    Q_FOREACH (MessageViewer::NetworkPluginUrlInterceptorInterface *interface, d->mManager->interfaceList()) {
        networkUrlInterceptor->addInterceptor(interface);
    }
    QWebEngineProfile::defaultProfile()->setRequestInterceptor(networkUrlInterceptor);
}

NetworkAccessManagerWebEngine::~NetworkAccessManagerWebEngine()
{

}
