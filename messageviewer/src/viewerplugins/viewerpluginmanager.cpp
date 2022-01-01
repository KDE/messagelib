/*
   SPDX-FileCopyrightText: 2015-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "viewerpluginmanager.h"
#include "messageviewer_debug.h"
#include "viewerplugin.h"

#include <KPluginFactory>
#include <KPluginMetaData>
#include <KSharedConfig>
#include <QFileInfo>

using namespace MessageViewer;

class ViewerPluginInfo
{
public:
    ViewerPluginInfo() = default;

    KPluginMetaData data;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PimCommon::PluginUtilData pluginData;
    MessageViewer::ViewerPlugin *plugin = nullptr;
    bool isEnabled = false;
};

class MessageViewer::ViewerPluginManagerPrivate
{
public:
    explicit ViewerPluginManagerPrivate(ViewerPluginManager *qq)
        : q(qq)
    {
    }

    bool initializePluginList();
    void loadPlugin(ViewerPluginInfo *item);
    Q_REQUIRED_RESULT QVector<MessageViewer::ViewerPlugin *> pluginsList() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginDataList() const;

    QString pluginDirectory;
    QString pluginName;

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    ViewerPlugin *pluginFromIdentifier(const QString &id);

private:
    QVector<ViewerPluginInfo> mPluginList;
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    ViewerPluginManager *const q;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("2.0");
}
}

QString ViewerPluginManagerPrivate::configGroupName() const
{
    return QStringLiteral("PluginMessageViewer%1").arg(pluginName);
}

QString ViewerPluginManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("MessageViewerPlugins");
}

bool ViewerPluginManagerPrivate::initializePluginList()
{
    if (!mPluginList.isEmpty()) {
        return true;
    }
    if (pluginDirectory.isEmpty()) {
        return false;
    }

    QVector<KPluginMetaData> plugins = KPluginMetaData::findPlugins(pluginDirectory);

    // We need common plugin to avoid to duplicate code between akregator/kmail
    plugins += KPluginMetaData::findPlugins(QStringLiteral("messageviewer/viewercommonplugin"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());
    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        ViewerPluginInfo info;

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
            qCWarning(MESSAGEVIEWER_LOG) << "Plugin name :" << data.name() << " doesn't have correct plugin version. Please update it";
        }
    }
    QVector<ViewerPluginInfo>::iterator end(mPluginList.end());
    for (QVector<ViewerPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    return true;
}

void ViewerPluginManagerPrivate::loadPlugin(ViewerPluginInfo *item)
{
    if (auto plugin = KPluginFactory::instantiatePlugin<MessageViewer::ViewerPlugin>(item->data, q, QVariantList() << item->metaDataFileName).plugin) {
        item->plugin = plugin;
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QVector<ViewerPlugin *> ViewerPluginManagerPrivate::pluginsList() const
{
    QVector<MessageViewer::ViewerPlugin *> lst;
    QVector<ViewerPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<ViewerPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

QVector<PimCommon::PluginUtilData> ViewerPluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

ViewerPlugin *ViewerPluginManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<ViewerPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<ViewerPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

ViewerPluginManager::ViewerPluginManager(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::ViewerPluginManagerPrivate(this))
{
}

MessageViewer::ViewerPluginManager::~ViewerPluginManager() = default;

bool ViewerPluginManager::initializePluginList()
{
    return d->initializePluginList();
}

ViewerPluginManager *ViewerPluginManager::self()
{
    static ViewerPluginManager s_self;
    return &s_self;
}

QVector<MessageViewer::ViewerPlugin *> ViewerPluginManager::pluginsList() const
{
    return d->pluginsList();
}

void ViewerPluginManager::setPluginDirectory(const QString &directory)
{
    d->pluginDirectory = directory;
}

QString ViewerPluginManager::pluginDirectory() const
{
    return d->pluginDirectory;
}

QVector<PimCommon::PluginUtilData> ViewerPluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

QString ViewerPluginManager::configGroupName() const
{
    return d->configGroupName();
}

QString ViewerPluginManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

MessageViewer::ViewerPlugin *ViewerPluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

void ViewerPluginManager::setPluginName(const QString &pluginName)
{
    d->pluginName = pluginName;
}

QString ViewerPluginManager::pluginName() const
{
    return d->pluginName;
}
