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

#include "mailnetworkpluginurlinterceptor.h"
#include "mailnetworkurlinterceptorpluginmanager.h"

#include <KPluginLoader>
#include <KPluginFactory>
#include <QFileInfo>
#include <QVariant>
#include <QSet>
#include <kpluginmetadata.h>

using namespace MessageViewer;

class MailNetworkUrlInterceptorPluginManagerInstancePrivate
{
public:
    MailNetworkUrlInterceptorPluginManagerInstancePrivate()
        : mailNetworkUrlInterceptorPluginManager(new NetworkUrlInterceptorPluginManager)
    {
    }

    ~MailNetworkUrlInterceptorPluginManagerInstancePrivate()
    {
        delete mailNetworkUrlInterceptorPluginManager;
    }

    NetworkUrlInterceptorPluginManager *mailNetworkUrlInterceptorPluginManager;
};

Q_GLOBAL_STATIC(MailNetworkUrlInterceptorPluginManagerInstancePrivate, sInstance)

class MailNetworkUrlInterceptorPluginInfo
{
public:
    MailNetworkUrlInterceptorPluginInfo()
        : plugin(Q_NULLPTR)
    {

    }
    QString saveName() const;

    KPluginMetaData metaData;
    MessageViewer::NetworkPluginUrlInterceptor *plugin;
};

QString MailNetworkUrlInterceptorPluginInfo::saveName() const
{
    return QFileInfo(metaData.fileName()).baseName();
}

namespace
{
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

class MessageViewer::NetworkUrlInterceptorPluginManagerPrivate
{
public:
    NetworkUrlInterceptorPluginManagerPrivate(NetworkUrlInterceptorPluginManager *qq)
        : q(qq)
    {

    }
    void initializePluginList();

    void loadPlugin(MailNetworkUrlInterceptorPluginInfo *item);
    QVector<MessageViewer::NetworkPluginUrlInterceptor *> pluginsList() const;

    QVector<MailNetworkUrlInterceptorPluginInfo> mPluginList;
    NetworkUrlInterceptorPluginManager *q;
};

void NetworkUrlInterceptorPluginManagerPrivate::initializePluginList()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("messageviewer"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("MessageViewer/UrlInterceptor"));
    });

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        MailNetworkUrlInterceptorPluginInfo info;
        info.metaData = i.previous();

        const QString version = info.metaData.version();
        if (pluginVersion() == version) {

            // only load plugins once, even if found multiple times!
            if (unique.contains(info.saveName())) {
                continue;
            }
            info.plugin = Q_NULLPTR;
            mPluginList.append(info);
            unique.insert(info.saveName());
        }
    }
    QVector<MailNetworkUrlInterceptorPluginInfo>::iterator end(mPluginList.end());
    for (QVector<MailNetworkUrlInterceptorPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QVector<MessageViewer::NetworkPluginUrlInterceptor *> NetworkUrlInterceptorPluginManagerPrivate::pluginsList() const
{
    QVector<MessageViewer::NetworkPluginUrlInterceptor *> lst;
    QVector<MailNetworkUrlInterceptorPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<MailNetworkUrlInterceptorPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).plugin) {
            lst << (*it).plugin;
        }
    }
    return lst;
}

void NetworkUrlInterceptorPluginManagerPrivate::loadPlugin(MailNetworkUrlInterceptorPluginInfo *item)
{
    item->plugin = KPluginLoader(item->metaData.fileName()).factory()->create<MessageViewer::NetworkPluginUrlInterceptor>(q, QVariantList() << item->saveName());
}

NetworkUrlInterceptorPluginManager *NetworkUrlInterceptorPluginManager::self()
{
    return sInstance->mailNetworkUrlInterceptorPluginManager;
}

NetworkUrlInterceptorPluginManager::NetworkUrlInterceptorPluginManager(QObject *parent)
    : QObject(parent),
      d(new NetworkUrlInterceptorPluginManagerPrivate(this))
{
    d->initializePluginList();
}

NetworkUrlInterceptorPluginManager::~NetworkUrlInterceptorPluginManager()
{
    delete d;
}

QVector<MessageViewer::NetworkPluginUrlInterceptor *> NetworkUrlInterceptorPluginManager::pluginsList() const
{
    return d->pluginsList();
}
