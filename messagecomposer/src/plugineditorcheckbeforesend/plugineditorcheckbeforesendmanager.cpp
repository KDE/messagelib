/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorcheckbeforesendmanager.h"
#include "messagecomposer_debug.h"
#include "plugineditorcheckbeforesend.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <QFileInfo>

using namespace MessageComposer;

class PluginEditorCheckBeforeSendInfo
{
public:
    PluginEditorCheckBeforeSendInfo() = default;

    KPluginMetaData data;
    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PluginEditorCheckBeforeSend *plugin = nullptr;
    bool isEnabled = true;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

class MessageComposer::PluginEditorCheckBeforeSendManagerPrivate
{
public:
    explicit PluginEditorCheckBeforeSendManagerPrivate(PluginEditorCheckBeforeSendManager *qq)
        : q(qq)
    {
        initializePlugins();
    }

    void loadPlugin(PluginEditorCheckBeforeSendInfo *item);
    [[nodiscard]] QList<PluginEditorCheckBeforeSend *> pluginsList() const;
    void initializePlugins();
    QList<PluginEditorCheckBeforeSendInfo> mPluginList;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginsDataList() const;
    PluginEditorCheckBeforeSend *pluginFromIdentifier(const QString &id);

private:
    QList<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorCheckBeforeSendManager *const q;
};

QString PluginEditorCheckBeforeSendManagerPrivate::configGroupName() const
{
    return QStringLiteral("KMailPluginCheckBefore");
}

QString PluginEditorCheckBeforeSendManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("PluginCheckBefore");
}

QList<PimCommon::PluginUtilData> PluginEditorCheckBeforeSendManagerPrivate::pluginsDataList() const
{
    return mPluginDataList;
}

void PluginEditorCheckBeforeSendManagerPrivate::initializePlugins()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/kmail/plugincheckbeforesend"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        PluginEditorCheckBeforeSendInfo info;
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
            qCWarning(MESSAGECOMPOSER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QList<PluginEditorCheckBeforeSendInfo>::iterator end(mPluginList.end());
    for (QList<PluginEditorCheckBeforeSendInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void PluginEditorCheckBeforeSendManagerPrivate::loadPlugin(PluginEditorCheckBeforeSendInfo *item)
{
    if (auto plugin = KPluginFactory::instantiatePlugin<PluginEditorCheckBeforeSend>(item->data, q, QVariantList() << item->metaDataFileName).plugin) {
        item->plugin = plugin;
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QList<PluginEditorCheckBeforeSend *> PluginEditorCheckBeforeSendManagerPrivate::pluginsList() const
{
    QList<PluginEditorCheckBeforeSend *> lst;
    QList<PluginEditorCheckBeforeSendInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<PluginEditorCheckBeforeSendInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

PluginEditorCheckBeforeSend *PluginEditorCheckBeforeSendManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QList<PluginEditorCheckBeforeSendInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<PluginEditorCheckBeforeSendInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

PluginEditorCheckBeforeSendManager::PluginEditorCheckBeforeSendManager(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorCheckBeforeSendManagerPrivate(this))
{
}

PluginEditorCheckBeforeSendManager::~PluginEditorCheckBeforeSendManager() = default;

PluginEditorCheckBeforeSendManager *PluginEditorCheckBeforeSendManager::self()
{
    static PluginEditorCheckBeforeSendManager s_self;
    return &s_self;
}

QList<PluginEditorCheckBeforeSend *> PluginEditorCheckBeforeSendManager::pluginsList() const
{
    return d->pluginsList();
}

QString PluginEditorCheckBeforeSendManager::configGroupName() const
{
    return d->configGroupName();
}

QString PluginEditorCheckBeforeSendManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QList<PimCommon::PluginUtilData> PluginEditorCheckBeforeSendManager::pluginsDataList() const
{
    return d->pluginsDataList();
}

PluginEditorCheckBeforeSend *PluginEditorCheckBeforeSendManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

#include "moc_plugineditorcheckbeforesendmanager.cpp"
