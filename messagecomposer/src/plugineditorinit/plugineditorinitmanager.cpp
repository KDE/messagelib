/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorinitmanager.h"
#include "messagecomposer_debug.h"
#include "plugineditorinit.h"
#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>
#include <QFileInfo>
#include <QSet>

using namespace MessageComposer;

class PluginEditorInitInfo
{
public:
    PluginEditorInitInfo()
    {
    }

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PluginEditorInit *plugin = nullptr;
    bool isEnabled = true;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

class MessageComposer::PluginEditorInitManagerPrivate
{
public:
    PluginEditorInitManagerPrivate(PluginEditorInitManager *qq)
        : q(qq)
    {
        initializePlugins();
    }

    void loadPlugin(PluginEditorInitInfo *item);
    Q_REQUIRED_RESULT QVector<PluginEditorInit *> pluginsList() const;
    void initializePlugins();
    QVector<PluginEditorInitInfo> mPluginList;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    PluginEditorInit *pluginFromIdentifier(const QString &id);

private:
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorInitManager *const q;
};

QString PluginEditorInitManagerPrivate::configGroupName() const
{
    return QStringLiteral("KMailPluginEditorInit");
}

QString PluginEditorInitManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("PluginEditorInit");
}

QVector<PimCommon::PluginUtilData> PluginEditorInitManagerPrivate::pluginsDataList() const
{
    return mPluginDataList;
}

void PluginEditorInitManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail/plugineditorinit"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        PluginEditorInitInfo info;
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
    QVector<PluginEditorInitInfo>::iterator end(mPluginList.end());
    for (QVector<PluginEditorInitInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void PluginEditorInitManagerPrivate::loadPlugin(PluginEditorInitInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<PluginEditorInit>(q, QVariantList() << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QVector<PluginEditorInit *> PluginEditorInitManagerPrivate::pluginsList() const
{
    QVector<PluginEditorInit *> lst;
    QVector<PluginEditorInitInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorInitInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

PluginEditorInit *PluginEditorInitManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<PluginEditorInitInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorInitInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

PluginEditorInitManager::PluginEditorInitManager(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorInitManagerPrivate(this))
{
}

PluginEditorInitManager::~PluginEditorInitManager()
{
    delete d;
}

PluginEditorInitManager *PluginEditorInitManager::self()
{
    static PluginEditorInitManager s_self;
    return &s_self;
}

QVector<PluginEditorInit *> PluginEditorInitManager::pluginsList() const
{
    return d->pluginsList();
}

QString PluginEditorInitManager::configGroupName() const
{
    return d->configGroupName();
}

QString PluginEditorInitManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QVector<PimCommon::PluginUtilData> PluginEditorInitManager::pluginsDataList() const
{
    return d->pluginsDataList();
}

PluginEditorInit *PluginEditorInitManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
