/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorcheckbeforesendmanager.h"
#include "messagecomposer_debug.h"
#include "plugineditorcheckbeforesend.h"
#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>
#include <QFileInfo>
#include <QSet>

using namespace MessageComposer;

class PluginEditorCheckBeforeSendInfo
{
public:
    PluginEditorCheckBeforeSendInfo()
    {
    }

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
    PluginEditorCheckBeforeSendManagerPrivate(PluginEditorCheckBeforeSendManager *qq)
        : q(qq)
    {
        initializePlugins();
    }

    void loadPlugin(PluginEditorCheckBeforeSendInfo *item);
    Q_REQUIRED_RESULT QVector<PluginEditorCheckBeforeSend *> pluginsList() const;
    void initializePlugins();
    QVector<PluginEditorCheckBeforeSendInfo> mPluginList;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    PluginEditorCheckBeforeSend *pluginFromIdentifier(const QString &id);

private:
    QVector<PimCommon::PluginUtilData> mPluginDataList;
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

QVector<PimCommon::PluginUtilData> PluginEditorCheckBeforeSendManagerPrivate::pluginsDataList() const
{
    return mPluginDataList;
}

void PluginEditorCheckBeforeSendManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail/plugincheckbeforesend"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
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
        if (pluginVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(MESSAGECOMPOSER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QVector<PluginEditorCheckBeforeSendInfo>::iterator end(mPluginList.end());
    for (QVector<PluginEditorCheckBeforeSendInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void PluginEditorCheckBeforeSendManagerPrivate::loadPlugin(PluginEditorCheckBeforeSendInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<PluginEditorCheckBeforeSend>(q, QVariantList() << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QVector<PluginEditorCheckBeforeSend *> PluginEditorCheckBeforeSendManagerPrivate::pluginsList() const
{
    QVector<PluginEditorCheckBeforeSend *> lst;
    QVector<PluginEditorCheckBeforeSendInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorCheckBeforeSendInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

PluginEditorCheckBeforeSend *PluginEditorCheckBeforeSendManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<PluginEditorCheckBeforeSendInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorCheckBeforeSendInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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

PluginEditorCheckBeforeSendManager::~PluginEditorCheckBeforeSendManager()
{
    delete d;
}

PluginEditorCheckBeforeSendManager *PluginEditorCheckBeforeSendManager::self()
{
    static PluginEditorCheckBeforeSendManager s_self;
    return &s_self;
}

QVector<PluginEditorCheckBeforeSend *> PluginEditorCheckBeforeSendManager::pluginsList() const
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

QVector<PimCommon::PluginUtilData> PluginEditorCheckBeforeSendManager::pluginsDataList() const
{
    return d->pluginsDataList();
}

PluginEditorCheckBeforeSend *PluginEditorCheckBeforeSendManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
