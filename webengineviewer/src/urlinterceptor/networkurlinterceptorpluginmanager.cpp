/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "networkurlinterceptorpluginmanager.h"
#include "networkpluginurlinterceptor.h"
#include "webengineviewer_debug.h"

#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>
#include <QFileInfo>
#include <QSet>
#include <QVariant>

using namespace WebEngineViewer;

class MailNetworkUrlInterceptorPluginInfo
{
public:
    MailNetworkUrlInterceptorPluginInfo()
    {
    }

    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PimCommon::PluginUtilData pluginData;
    WebEngineViewer::NetworkPluginUrlInterceptor *plugin = nullptr;
    bool isEnabled = false;
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
    Q_REQUIRED_RESULT QVector<WebEngineViewer::NetworkPluginUrlInterceptor *> pluginsList() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginDataList() const;
    Q_REQUIRED_RESULT NetworkPluginUrlInterceptor *pluginFromIdentifier(const QString &id);

private:
    QVector<MailNetworkUrlInterceptorPluginInfo> mPluginList;
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    NetworkUrlInterceptorPluginManager *const q;
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
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("webengineviewer/urlinterceptor"));

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    while (i.hasPrevious()) {
        MailNetworkUrlInterceptorPluginInfo info;

        const KPluginMetaData data = i.previous();

        // 1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        // 2) look at if plugin is activated
        const bool isPluginActivated =
            PimCommon::PluginUtil::isPluginActivated(pair.first, pair.second, info.pluginData.mEnableByDefault, info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();

        if (pluginVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
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
        if (auto plugin = (*it).plugin) {
            lst << plugin;
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

WebEngineViewer::NetworkPluginUrlInterceptor *NetworkUrlInterceptorPluginManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<MailNetworkUrlInterceptorPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<MailNetworkUrlInterceptorPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

NetworkUrlInterceptorPluginManager *NetworkUrlInterceptorPluginManager::self()
{
    static NetworkUrlInterceptorPluginManager s_self;
    return &s_self;
}

NetworkUrlInterceptorPluginManager::NetworkUrlInterceptorPluginManager(QObject *parent)
    : QObject(parent)
    , d(new NetworkUrlInterceptorPluginManagerPrivate(this))
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

WebEngineViewer::NetworkPluginUrlInterceptor *NetworkUrlInterceptorPluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
