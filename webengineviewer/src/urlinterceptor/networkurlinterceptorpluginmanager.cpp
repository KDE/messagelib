/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "networkurlinterceptorpluginmanager.h"
#include "networkpluginurlinterceptor.h"
#include "webengineviewer_debug.h"

#include <KPluginFactory>
#include <KPluginMetaData>
#include <QFileInfo>
#include <QVariant>

using namespace WebEngineViewer;

class MailNetworkUrlInterceptorPluginInfo
{
public:
    MailNetworkUrlInterceptorPluginInfo() = default;

    KPluginMetaData data;
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
    explicit NetworkUrlInterceptorPluginManagerPrivate(NetworkUrlInterceptorPluginManager *qq)
        : q(qq)
    {
    }

    void initializePluginList();

    void loadPlugin(MailNetworkUrlInterceptorPluginInfo *item);
    [[nodiscard]] QList<WebEngineViewer::NetworkPluginUrlInterceptor *> pluginsList() const;
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginDataList() const;
    [[nodiscard]] NetworkPluginUrlInterceptor *pluginFromIdentifier(const QString &id);

private:
    QList<MailNetworkUrlInterceptorPluginInfo> mPluginList;
    QList<PimCommon::PluginUtilData> mPluginDataList;
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

QList<PimCommon::PluginUtilData> NetworkUrlInterceptorPluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

void NetworkUrlInterceptorPluginManagerPrivate::initializePluginList()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/webengineviewer/urlinterceptor"));

    QListIterator<KPluginMetaData> i(plugins);
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
        info.data = data;

        if (pluginVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(WEBENGINEVIEWER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QList<MailNetworkUrlInterceptorPluginInfo>::iterator end(mPluginList.end());
    for (QList<MailNetworkUrlInterceptorPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QList<WebEngineViewer::NetworkPluginUrlInterceptor *> NetworkUrlInterceptorPluginManagerPrivate::pluginsList() const
{
    QList<WebEngineViewer::NetworkPluginUrlInterceptor *> lst;
    QList<MailNetworkUrlInterceptorPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<MailNetworkUrlInterceptorPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

void NetworkUrlInterceptorPluginManagerPrivate::loadPlugin(MailNetworkUrlInterceptorPluginInfo *item)
{
    if (auto plugin =
            KPluginFactory::instantiatePlugin<WebEngineViewer::NetworkPluginUrlInterceptor>(item->data, q, QVariantList() << item->metaDataFileName).plugin) {
        item->plugin = plugin;
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

WebEngineViewer::NetworkPluginUrlInterceptor *NetworkUrlInterceptorPluginManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QList<MailNetworkUrlInterceptorPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<MailNetworkUrlInterceptorPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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

NetworkUrlInterceptorPluginManager::~NetworkUrlInterceptorPluginManager() = default;

QList<WebEngineViewer::NetworkPluginUrlInterceptor *> NetworkUrlInterceptorPluginManager::pluginsList() const
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

QList<PimCommon::PluginUtilData> NetworkUrlInterceptorPluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

WebEngineViewer::NetworkPluginUrlInterceptor *NetworkUrlInterceptorPluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

#include "moc_networkurlinterceptorpluginmanager.cpp"
