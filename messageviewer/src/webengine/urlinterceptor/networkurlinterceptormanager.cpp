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

#include "networkurlinterceptormanager.h"
#include "networkurlinterceptorpluginmanager.h"

#include <QWebEngineProfile>

using namespace MessageViewer;

class MessageViewer::NetworkUrlInterceptorManagerPrivate
{
public:
    NetworkUrlInterceptorManagerPrivate(KActionCollection *ac, NetworkUrlInterceptorManager *qq)
        : q(qq)
    {
        createInterfaces(ac);
    }
    void createInterfaces(KActionCollection *ac);
    QVector<MessageViewer::NetworkPluginUrlInterceptorInterface *> mListInterface;
    NetworkUrlInterceptorManager *q;
};

void NetworkUrlInterceptorManagerPrivate::createInterfaces(KActionCollection *ac)
{
    Q_FOREACH (NetworkPluginUrlInterceptor *plugin, NetworkUrlInterceptorPluginManager::self()->pluginsList()) {
        MessageViewer::NetworkPluginUrlInterceptorInterface *interface = plugin->createInterface(q);
        plugin->createActions(ac);
        mListInterface.append(interface);
    }
}

NetworkUrlInterceptorManager::NetworkUrlInterceptorManager(KActionCollection *ac, QObject *parent)
    : QObject(parent),
      d(new NetworkUrlInterceptorManagerPrivate(ac, this))
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

