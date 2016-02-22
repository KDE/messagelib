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

#include "mailnetworkurlinterceptormanager.h"
#include "mailnetworkurlinterceptorpluginmanager.h"

#include <QWebEngineProfile>

using namespace MessageViewer;

class MessageViewer::MailNetworkUrlInterceptorManagerPrivate
{
public:
    MailNetworkUrlInterceptorManagerPrivate(MailNetworkUrlInterceptorManager *qq)
        : q(qq)
    {
        createInterfaces();
    }
    void createInterfaces();
    QVector<MessageViewer::MailNetworkPluginUrlInterceptorInterface *> mListInterface;
    MailNetworkUrlInterceptorManager *q;
};

void MailNetworkUrlInterceptorManagerPrivate::createInterfaces()
{
    Q_FOREACH (MailNetworkPluginUrlInterceptor *plugin, MailNetworkUrlInterceptorPluginManager::self()->pluginsList()) {
        MessageViewer::MailNetworkPluginUrlInterceptorInterface *interface = plugin->createInterface(q);
        mListInterface.append(interface);
    }
}

MailNetworkUrlInterceptorManager::MailNetworkUrlInterceptorManager(QObject *parent)
    : QObject(parent),
      d(new MailNetworkUrlInterceptorManagerPrivate(this))
{

}

MailNetworkUrlInterceptorManager::~MailNetworkUrlInterceptorManager()
{
    delete d;
}

QVector<MessageViewer::MailNetworkPluginUrlInterceptorInterface *> MailNetworkUrlInterceptorManager::interfaceList() const
{
    return d->mListInterface;
}

