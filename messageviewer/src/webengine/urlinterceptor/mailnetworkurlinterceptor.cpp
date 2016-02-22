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

#include "mailnetworkpluginurlinterceptorinterface.h"
#include "mailnetworkurlinterceptor.h"
#include "messageviewer_debug.h"

#include <QVector>

using namespace MessageViewer;

class MessageViewer::NetworkUrlInterceptorPrivate
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
        qCDebug(MESSAGEVIEWER_LOG) << "interceptor already added. This is a problem" << interceptor;
    }
}

void NetworkUrlInterceptorPrivate::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    Q_FOREACH (NetworkPluginUrlInterceptorInterface *inter, listInterceptor) {
        inter->interceptRequest(info);
    }
}

NetworkUrlInterceptor::NetworkUrlInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent),
      d(new NetworkUrlInterceptorPrivate)
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

