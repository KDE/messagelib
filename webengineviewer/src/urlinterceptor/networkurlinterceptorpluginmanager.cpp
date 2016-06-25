/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "networkpluginurlinterceptor.h"
#include "networkurlinterceptorpluginmanager.h"
#include "webengineviewer_debug.h"

#include <KPluginLoader>
#include <KPluginFactory>
#include <QFileInfo>
#include <QVariant>
#include <QSet>
#include <kpluginmetadata.h>

using namespace WebEngineViewer;

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
    WebEngineViewer::NetworkPluginUrlInterceptor *plugin;
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

class WebEngineViewer::NetworkUrlInterceptorPluginManagerPrivate
{
public:
    NetworkUrlInterceptorPluginManagerPrivate(NetworkUrlInterceptorPluginManager *qq)
        : q(qq)
    {

    }
    void initializePluginList();

    void loadPlugin(MailNetworkUrlInterceptorPluginInfo *item);
    QVector<WebEngineViewer::NetworkPluginUrlInterceptor *> pluginsList() const;

    QVector<MailNetworkUrlInterceptorPluginInfo> mPluginList;
    NetworkUrlInterceptorPluginManager *q;
};

void NetworkUrlInterceptorPluginManagerPrivate::initializePluginList()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("webengineviewer"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("WebEngineViewer/UrlInterceptor"));
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
        } else {
            qCWarning(WEBENGINEVIEWER_LOG) << "Plugin " << info.metaData.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QVector<MailNetworkUrlInterceptorPluginInfo>::iterator end(mPluginList.end());
    for (QVector<MailNetworkUrlInterceptorPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QVector<WebEngineViewer::NetworkPluginUrlInterceptor *> NetworkUrlInterceptorPluginManagerPrivate::pluginsList() const
{
    QVector<WebEngineViewer::NetworkPluginUrlInterceptor *> lst;
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
    KPluginLoader pluginLoader(item->metaData.fileName());
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<WebEngineViewer::NetworkPluginUrlInterceptor>(q, QVariantList() << item->saveName());
    }
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

QVector<WebEngineViewer::NetworkPluginUrlInterceptor *> NetworkUrlInterceptorPluginManager::pluginsList() const
{
    return d->pluginsList();
}
