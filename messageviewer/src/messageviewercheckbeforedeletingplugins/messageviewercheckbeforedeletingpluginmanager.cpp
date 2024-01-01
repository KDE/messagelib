/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewercheckbeforedeletingpluginmanager.h"
#include "messageviewer_debug.h"
#include "messageviewercheckbeforedeletingplugin.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <QFileInfo>

using namespace MessageViewer;

class CheckBeforeDeletingPluginInfo
{
public:
    CheckBeforeDeletingPluginInfo() = default;

    KPluginMetaData data;
    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    bool isEnabled = true;
    MessageViewer::MessageViewerCheckBeforeDeletingPlugin *plugin = nullptr;
};

class MessageViewer::MessageViewerCheckBeforeDeletingPluginManagerPrivate
{
public:
    explicit MessageViewerCheckBeforeDeletingPluginManagerPrivate(MessageViewerCheckBeforeDeletingPluginManager *qq)
        : q(qq)
    {
    }

    [[nodiscard]] QList<MessageViewer::MessageViewerCheckBeforeDeletingPlugin *> pluginsList() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginDataList() const;
    void initializePluginList();
    void loadPlugin(CheckBeforeDeletingPluginInfo *item);
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] MessageViewerCheckBeforeDeletingPlugin *pluginFromIdentifier(const QString &id);

private:
    QList<PimCommon::PluginUtilData> mPluginDataList;
    QList<CheckBeforeDeletingPluginInfo> mPluginList;
    MessageViewerCheckBeforeDeletingPluginManager *const q;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

QList<PimCommon::PluginUtilData> MessageViewerCheckBeforeDeletingPluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

QString MessageViewerCheckBeforeDeletingPluginManagerPrivate::configGroupName() const
{
    return QStringLiteral("MessageViewerCheckBeforeDeletingPlugins");
}

QString MessageViewerCheckBeforeDeletingPluginManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("MessageViewerCheckBeforeDeletingPlugin");
}

void MessageViewerCheckBeforeDeletingPluginManagerPrivate::initializePluginList()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/messageviewer/checkbeforedeleting"));
    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());
    QList<int> listOrder;
    while (i.hasPrevious()) {
        CheckBeforeDeletingPluginInfo info;

        const KPluginMetaData data = i.previous();

        // 1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        // 2) look at if plugin is activated
        const bool isPluginActivated =
            PimCommon::PluginUtil::isPluginActivated(pair.first, pair.second, info.pluginData.mEnableByDefault, info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.data = data;

        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        const QString version = data.version();
        if (pluginVersion() == version) {
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
    QList<CheckBeforeDeletingPluginInfo>::iterator end(mPluginList.end());
    for (QList<CheckBeforeDeletingPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QList<MessageViewer::MessageViewerCheckBeforeDeletingPlugin *> MessageViewerCheckBeforeDeletingPluginManagerPrivate::pluginsList() const
{
    QList<MessageViewer::MessageViewerCheckBeforeDeletingPlugin *> lst;
    QList<CheckBeforeDeletingPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<CheckBeforeDeletingPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

void MessageViewerCheckBeforeDeletingPluginManagerPrivate::loadPlugin(CheckBeforeDeletingPluginInfo *item)
{
    if (auto plugin =
            KPluginFactory::instantiatePlugin<MessageViewer::MessageViewerCheckBeforeDeletingPlugin>(item->data, q, QVariantList() << item->metaDataFileName)
                .plugin) {
        item->plugin = plugin;
        // By default it's true
        item->pluginData.mHasConfigureDialog = true;
        item->plugin->setIsEnabled(item->isEnabled);
        mPluginDataList.append(item->pluginData);
    }
}

MessageViewerCheckBeforeDeletingPlugin *MessageViewerCheckBeforeDeletingPluginManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QList<CheckBeforeDeletingPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<CheckBeforeDeletingPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

MessageViewerCheckBeforeDeletingPluginManager *MessageViewerCheckBeforeDeletingPluginManager::self()
{
    static MessageViewerCheckBeforeDeletingPluginManager s_self;
    return &s_self;
}

MessageViewerCheckBeforeDeletingPluginManager::MessageViewerCheckBeforeDeletingPluginManager(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerCheckBeforeDeletingPluginManagerPrivate(this))
{
    d->initializePluginList();
}

MessageViewerCheckBeforeDeletingPluginManager::~MessageViewerCheckBeforeDeletingPluginManager() = default;

QList<MessageViewer::MessageViewerCheckBeforeDeletingPlugin *> MessageViewerCheckBeforeDeletingPluginManager::pluginsList() const
{
    return d->pluginsList();
}

QString MessageViewerCheckBeforeDeletingPluginManager::configGroupName() const
{
    return d->configGroupName();
}

QString MessageViewerCheckBeforeDeletingPluginManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QList<PimCommon::PluginUtilData> MessageViewerCheckBeforeDeletingPluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

MessageViewerCheckBeforeDeletingPlugin *MessageViewerCheckBeforeDeletingPluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

#include "moc_messageviewercheckbeforedeletingpluginmanager.cpp"
