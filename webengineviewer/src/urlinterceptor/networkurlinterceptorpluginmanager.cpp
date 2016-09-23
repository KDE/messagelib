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

    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PimCommon::PluginUtilData pluginData;
    WebEngineViewer::NetworkPluginUrlInterceptor *plugin;
    bool isEnabled;
};

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
    QString configGroupName() const;
    QString configPrefixSettingKey() const;
    QVector<PimCommon::PluginUtilData> pluginDataList() const;
private:
    QVector<MailNetworkUrlInterceptorPluginInfo> mPluginList;
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    NetworkUrlInterceptorPluginManager *q;
};

QString NetworkUrlInterceptorPluginManagerPrivate::configGroupName() const
{
    return QStringLiteral("NetworkUrlInterceptorPlugins");
}

QString NetworkUrlInterceptorPluginManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("PluginsNetworkUrlInterceptor");
}

QVector<PimCommon::PluginUtilData> NetworkUrlInterceptorPluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

void NetworkUrlInterceptorPluginManagerPrivate::initializePluginList()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("webengineviewer"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("WebEngineViewer/UrlInterceptor"));
    });

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    while (i.hasPrevious()) {
        MailNetworkUrlInterceptorPluginInfo info;

        const KPluginMetaData data = i.previous();

        //1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        //2) look at if plugin is activated
        const bool isPluginActivated = PimCommon::PluginUtil::isPluginActivated(pair.first, pair.second, info.pluginData.mEnableByDefault, info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();

        if (pluginVersion() == data.version()) {
            // only load plugins once, even if found multiple times!
            if (unique.contains(info.metaDataFileNameBaseName)) {
                continue;
            }
            info.plugin = Q_NULLPTR;
            mPluginList.push_back(info);
            unique.insert(info.metaDataFileNameBaseName);
        } else {
            qCWarning(WEBENGINEVIEWER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
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
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<WebEngineViewer::NetworkPluginUrlInterceptor>(q, QVariantList() << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
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

QString NetworkUrlInterceptorPluginManager::configGroupName() const
{
    return d->configGroupName();
}

QString NetworkUrlInterceptorPluginManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QVector<PimCommon::PluginUtilData> NetworkUrlInterceptorPluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}
