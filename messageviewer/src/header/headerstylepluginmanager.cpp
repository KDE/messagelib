/*
   SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "headerstylepluginmanager.h"
using namespace Qt::Literals::StringLiterals;

#include "headerstyleplugin.h"
#include "messageviewer_debug.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <PimCommon/PluginUtil>
#include <QFileInfo>

using namespace MessageViewer;

class HeaderStylePluginInfo
{
public:
    HeaderStylePluginInfo() = default;

    TextAddonsWidgets::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    KPluginMetaData data;
    MessageViewer::HeaderStylePlugin *plugin = nullptr;
    bool isEnabled = false;
};

class MessageViewer::HeaderStylePluginManagerPrivate
{
public:
    explicit HeaderStylePluginManagerPrivate(HeaderStylePluginManager *qq)
        : q(qq)
    {
    }

    [[nodiscard]] QList<MessageViewer::HeaderStylePlugin *> pluginsList() const;
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginDataList() const;
    void initializePluginList();
    void loadPlugin(HeaderStylePluginInfo *item);
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    MessageViewer::HeaderStylePlugin *pluginFromIdentifier(const QString &id);

private:
    QList<TextAddonsWidgets::PluginUtilData> mPluginDataList;
    QList<HeaderStylePluginInfo> mPluginList;
    HeaderStylePluginManager *const q;
};

namespace
{
QString pluginVersion()
{
    return u"1.0"_s;
}
}

QList<TextAddonsWidgets::PluginUtilData> HeaderStylePluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

QString HeaderStylePluginManagerPrivate::configGroupName() const
{
    return u"HeaderStylePlugins"_s;
}

QString HeaderStylePluginManagerPrivate::configPrefixSettingKey() const
{
    return u"PluginHeaderStyle"_s;
}

void HeaderStylePluginManagerPrivate::initializePluginList()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(u"pim6/messageviewer/headerstyle"_s);

    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    const TextAddonsWidgets::PluginUtil::PluginsStateList pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());
    QList<int> listOrder;
    while (i.hasPrevious()) {
        HeaderStylePluginInfo info;

        const KPluginMetaData data = i.previous();

        // 1) get plugin data => name/description etc.
        info.pluginData = TextAddonsWidgets::PluginUtil::createPluginMetaData(data);
        // 2) look at if plugin is activated
        const bool isPluginActivated = TextAddonsWidgets::PluginUtil::isPluginActivated(pair.enabledPluginList,
                                                                                        pair.disabledPluginList,
                                                                                        info.pluginData.mEnableByDefault,
                                                                                        info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        info.data = data;
        const QString version = data.version();
        if (pluginVersion() == version) {
            const QVariant p = data.rawData().value(u"X-KDE-MessageViewer-Header-Order"_s).toVariant();
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

QList<TextAddonsWidgets::PluginUtilData> HeaderStylePluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

MessageViewer::HeaderStylePlugin *HeaderStylePluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

#include "moc_headerstylepluginmanager.cpp"
