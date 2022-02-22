/*
   SPDX-FileCopyrightText: 2015-2022 Laurent Montel <montel@kde.org>

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

    Q_REQUIRED_RESULT QVector<MessageViewer::HeaderStylePlugin *> pluginsList() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginDataList() const;
    void initializePluginList();
    void loadPlugin(HeaderStylePluginInfo *item);
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    MessageViewer::HeaderStylePlugin *pluginFromIdentifier(const QString &id);

private:
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    QVector<HeaderStylePluginInfo> mPluginList;
    HeaderStylePluginManager *const q;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

QVector<PimCommon::PluginUtilData> HeaderStylePluginManagerPrivate::pluginDataList() const
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
    const QVector<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("messageviewer/headerstyle"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QVectorIterator<KPluginMetaData> i(plugins);
#else
    QListIterator<KPluginMetaData> i(plugins);
#endif
    i.toBack();
    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());
    QVector<int> listOrder;
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
    QVector<HeaderStylePluginInfo>::iterator end(mPluginList.end());
    for (QVector<HeaderStylePluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QVector<MessageViewer::HeaderStylePlugin *> HeaderStylePluginManagerPrivate::pluginsList() const
{
    QVector<MessageViewer::HeaderStylePlugin *> lst;
    QVector<HeaderStylePluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<HeaderStylePluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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
    QVector<HeaderStylePluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<HeaderStylePluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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

QVector<MessageViewer::HeaderStylePlugin *> HeaderStylePluginManager::pluginsList() const
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

QVector<PimCommon::PluginUtilData> HeaderStylePluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

MessageViewer::HeaderStylePlugin *HeaderStylePluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
