/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewerconfiguresettingspluginmanager.h"
#include "messageviewer_debug.h"
#include "messageviewerconfiguresettingsplugin.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <QFileInfo>

using namespace MessageViewer;

class ConfigureSettingsPluginInfo
{
public:
    ConfigureSettingsPluginInfo() = default;

    KPluginMetaData data;
    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    MessageViewer::MessageViewerConfigureSettingsPlugin *plugin = nullptr;
};

class MessageViewer::MessageViewerConfigureSettingsPluginManagerPrivate
{
public:
    explicit MessageViewerConfigureSettingsPluginManagerPrivate(MessageViewerConfigureSettingsPluginManager *qq)
        : q(qq)
    {
    }

    [[nodiscard]] QList<MessageViewer::MessageViewerConfigureSettingsPlugin *> pluginsList() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginDataList() const;
    void initializePluginList();
    void loadPlugin(ConfigureSettingsPluginInfo *item);
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    MessageViewerConfigureSettingsPlugin *pluginFromIdentifier(const QString &id);

private:
    QList<PimCommon::PluginUtilData> mPluginDataList;
    QList<ConfigureSettingsPluginInfo> mPluginList;
    MessageViewerConfigureSettingsPluginManager *const q;
};

namespace
{
QString configurePluginVersion()
{
    return QStringLiteral("1.0");
}
}

QList<PimCommon::PluginUtilData> MessageViewerConfigureSettingsPluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

QString MessageViewerConfigureSettingsPluginManagerPrivate::configGroupName() const
{
    return QStringLiteral("MessageViewerConfigureSettingsPlugins");
}

QString MessageViewerConfigureSettingsPluginManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("MessageViewerConfigureSettingsPlugin");
}

void MessageViewerConfigureSettingsPluginManagerPrivate::initializePluginList()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/messageviewer/configuresettings"));

    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());
    QList<int> listOrder;
    while (i.hasPrevious()) {
        ConfigureSettingsPluginInfo info;

        const KPluginMetaData data = i.previous();

        // 1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        // 2) look at if plugin is activated
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        info.data = data;
        const QString version = data.version();
        if (configurePluginVersion() == version) {
            const QVariant p = data.rawData().value(QStringLiteral("X-KDE-MessageViewer-Configure-Order")).toVariant();
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
    QList<ConfigureSettingsPluginInfo>::iterator end(mPluginList.end());
    for (QList<ConfigureSettingsPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QList<MessageViewer::MessageViewerConfigureSettingsPlugin *> MessageViewerConfigureSettingsPluginManagerPrivate::pluginsList() const
{
    QList<MessageViewer::MessageViewerConfigureSettingsPlugin *> lst;
    QList<ConfigureSettingsPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<ConfigureSettingsPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

void MessageViewerConfigureSettingsPluginManagerPrivate::loadPlugin(ConfigureSettingsPluginInfo *item)
{
    if (auto plugin =
            KPluginFactory::instantiatePlugin<MessageViewer::MessageViewerConfigureSettingsPlugin>(item->data, q, QVariantList() << item->metaDataFileName)
                .plugin) {
        item->plugin = plugin;
        // By default it's true
        item->pluginData.mHasConfigureDialog = true;
        mPluginDataList.append(item->pluginData);
    }
}

MessageViewerConfigureSettingsPlugin *MessageViewerConfigureSettingsPluginManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QList<ConfigureSettingsPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<ConfigureSettingsPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

MessageViewerConfigureSettingsPluginManager *MessageViewerConfigureSettingsPluginManager::self()
{
    static MessageViewerConfigureSettingsPluginManager s_self;
    return &s_self;
}

MessageViewerConfigureSettingsPluginManager::MessageViewerConfigureSettingsPluginManager(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerConfigureSettingsPluginManagerPrivate(this))
{
    d->initializePluginList();
}

MessageViewerConfigureSettingsPluginManager::~MessageViewerConfigureSettingsPluginManager() = default;

QList<MessageViewer::MessageViewerConfigureSettingsPlugin *> MessageViewerConfigureSettingsPluginManager::pluginsList() const
{
    return d->pluginsList();
}

QString MessageViewerConfigureSettingsPluginManager::configGroupName() const
{
    return d->configGroupName();
}

QString MessageViewerConfigureSettingsPluginManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QList<PimCommon::PluginUtilData> MessageViewerConfigureSettingsPluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

MessageViewerConfigureSettingsPlugin *MessageViewerConfigureSettingsPluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

#include "moc_messageviewerconfiguresettingspluginmanager.cpp"
