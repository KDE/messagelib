/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "headerstylepluginmanager.h"
#include "headerstyleplugin.h"
#include "messageviewer_debug.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <QFileInfo>

using namespace MessageViewer;

class HeaderStylePluginInfo
{
public:
    HeaderStylePluginInfo() = default;

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    KPluginMetaData data;
    MessageViewer::HeaderStylePlugin *plugin = nullptr;
    bool isEnabled = false;
};

class MessageViewer::HeaderStylePluginManagerPrivate
{
public:
    HeaderStylePluginManagerPrivate(HeaderStylePluginManager *qq)
        : q(qq)
    {
    }

    [[nodiscard]] QList<MessageViewer::HeaderStylePlugin *> pluginsList() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginDataList() const;
    void initializePluginList();
    void loadPlugin(HeaderStylePluginInfo *item);
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    MessageViewer::HeaderStylePlugin *pluginFromIdentifier(const QString &id);

private:
    QList<PimCommon::PluginUtilData> mPluginDataList;
    QList<HeaderStylePluginInfo> mPluginList;
    HeaderStylePluginManager *const q;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

QList<PimCommon::PluginUtilData> HeaderStylePluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

QString HeaderStylePluginManagerPrivate::configGroupName() const
{
    return QStringLiteral("HeaderStylePlugins");
}

QString HeaderStylePluginManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("PluginHeaderStyle");
}

void HeaderStylePluginManagerPrivate::initializePluginList()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/messageviewer/headerstyle"));

    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());
    QList<int> listOrder;
    while (i.hasPrevious()) {
        HeaderStylePluginInfo info;

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
        const QString version = data.version();
        if (pluginVersion() == version) {
            const QVariant p = data.rawData().value(QStringLiteral("X-KDE-MessageViewer-Header-Order")).toVariant();
            int order = -1;
            if (p.isValid()) {
                order = p.toInt();
            }
            int pos = 0;
            for (; pos < listOrder.count(); ++pos) {
                if (listOrder.at(pos) > order) {
                    pos--;
                    break;
                }
            }
            pos = qMax(0, pos);
            listOrder.insert(pos, order);
            info.plugin = nullptr;
            mPluginList.insert(pos, info);
        } else {
            qCWarning(MESSAGEVIEWER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QList<HeaderStylePluginInfo>::iterator end(mPluginList.end());
    for (QList<HeaderStylePluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QList<MessageViewer::HeaderStylePlugin *> HeaderStylePluginManagerPrivate::pluginsList() const
{
    QList<MessageViewer::HeaderStylePlugin *> lst;
    QList<HeaderStylePluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<HeaderStylePluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

void HeaderStylePluginManagerPrivate::loadPlugin(HeaderStylePluginInfo *item)
{
    if (auto plugin = KPluginFactory::instantiatePlugin<MessageViewer::HeaderStylePlugin>(item->data, q, QVariantList() << item->metaDataFileName).plugin) {
        item->plugin = plugin;
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = false;
        mPluginDataList.append(item->pluginData);
    }
}

MessageViewer::HeaderStylePlugin *HeaderStylePluginManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QList<HeaderStylePluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<HeaderStylePluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

HeaderStylePluginManager *HeaderStylePluginManager::self()
{
    static HeaderStylePluginManager s_self;
    return &s_self;
}

HeaderStylePluginManager::HeaderStylePluginManager(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::HeaderStylePluginManagerPrivate(this))
{
    d->initializePluginList();
}

HeaderStylePluginManager::~HeaderStylePluginManager() = default;

QList<MessageViewer::HeaderStylePlugin *> HeaderStylePluginManager::pluginsList() const
{
    return d->pluginsList();
}

QStringList HeaderStylePluginManager::pluginListName() const
{
    QStringList lst;
    lst.reserve(d->pluginsList().count());
    const auto pluginsList{d->pluginsList()};
    for (MessageViewer::HeaderStylePlugin *plugin : pluginsList) {
        lst << plugin->name();
    }
    return lst;
}

QString HeaderStylePluginManager::configGroupName() const
{
    return d->configGroupName();
}

QString HeaderStylePluginManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QList<PimCommon::PluginUtilData> HeaderStylePluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

MessageViewer::HeaderStylePlugin *HeaderStylePluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

#include "moc_headerstylepluginmanager.cpp"
