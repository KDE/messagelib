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

#include "mailnetworkpluginulrinterceptorinterface.h"
#include "mailnetworkulrinterceptor.h"
#include "messageviewer_debug.h"

#include <QVector>

using namespace MessageViewer;

class MessageViewer::MailNetworkUlrInterceptorPrivate
{
public:
    MailNetworkUlrInterceptorPrivate()
    {

    }
    void addInterceptor(MailNetworkPluginUlrInterceptorInterface *interceptor);
    void interceptRequest(QWebEngineUrlRequestInfo &info);
    QVector<MailNetworkPluginUlrInterceptorInterface *> listInterceptor;
};

void MailNetworkUlrInterceptorPrivate::addInterceptor(MailNetworkPluginUlrInterceptorInterface *interceptor)
{
    if (!listInterceptor.contains(interceptor)) {
        listInterceptor.append(interceptor);
    } else {
        qCDebug(MESSAGEVIEWER_LOG) << "interceptor already added. This is a problem" << interceptor;
    }
}

void MailNetworkUlrInterceptorPrivate::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    Q_FOREACH(MailNetworkPluginUlrInterceptorInterface *inter, listInterceptor) {
        inter->interceptRequest(info);
    }
}

MailNetworkUlrInterceptor::MailNetworkUlrInterceptor(QObject *parent)
    : QWebEngineUrlRequestInterceptor(parent),
      d(new MailNetworkUlrInterceptorPrivate)
{

}

MailNetworkUlrInterceptor::~MailNetworkUlrInterceptor()
{
    delete d;
}

void MailNetworkUlrInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    d->interceptRequest(info);
}

void MailNetworkUlrInterceptor::addInterceptor(MailNetworkPluginUlrInterceptorInterface *interceptor)
{
    d->addInterceptor(interceptor);
}

