/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorgrammarmanager.h"
#include "messagecomposer_debug.h"
#include "plugineditorgrammarcustomtoolsviewinterface.h"
#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>
#include <PimCommon/CustomToolsPlugin>
#include <QFileInfo>
#include <QSet>

using namespace MessageComposer;

class PluginEditorGrammarInfo
{
public:
    PluginEditorGrammarInfo()
    {
    }

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PimCommon::CustomToolsPlugin *plugin = nullptr;
    bool isEnabled = true;
};

namespace
{
QString pluginEditorGrammarVersion()
{
    return QStringLiteral("1.0");
}
}

class MessageComposer::PluginEditorGrammarManagerPrivate
{
public:
    PluginEditorGrammarManagerPrivate(PluginEditorGrammarManager *qq)
        : q(qq)
    {
        initializePlugins();
    }

    void loadPlugin(PluginEditorGrammarInfo *item);
    QVector<PimCommon::CustomToolsPlugin *> pluginsList() const;
    void initializePlugins();
    QVector<PluginEditorGrammarInfo> mPluginList;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    PimCommon::CustomToolsPlugin *pluginFromIdentifier(const QString &id);

private:
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorGrammarManager *const q;
};

QString PluginEditorGrammarManagerPrivate::configGroupName() const
{
    return QStringLiteral("KMailPluginEditorGrammar");
}

QString PluginEditorGrammarManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("PluginEditorGrammar");
}

QVector<PimCommon::PluginUtilData> PluginEditorGrammarManagerPrivate::pluginsDataList() const
{
    return mPluginDataList;
}

void PluginEditorGrammarManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail/plugineditorgrammar"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        PluginEditorGrammarInfo info;
        const KPluginMetaData data = i.previous();

        // 1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        // 2) look at if plugin is activated
        const bool isPluginActivated =
            PimCommon::PluginUtil::isPluginActivated(pair.first, pair.second, info.pluginData.mEnableByDefault, info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        if (pluginEditorGrammarVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(MESSAGECOMPOSER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QVector<PluginEditorGrammarInfo>::iterator end(mPluginList.end());
    for (QVector<PluginEditorGrammarInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void PluginEditorGrammarManagerPrivate::loadPlugin(PluginEditorGrammarInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<PimCommon::CustomToolsPlugin>(q, QVariantList() << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QVector<PimCommon::CustomToolsPlugin *> PluginEditorGrammarManagerPrivate::pluginsList() const
{
    QVector<PimCommon::CustomToolsPlugin *> lst;
    QVector<PluginEditorGrammarInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorGrammarInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

PimCommon::CustomToolsPlugin *PluginEditorGrammarManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<PluginEditorGrammarInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorGrammarInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

PluginEditorGrammarManager::PluginEditorGrammarManager(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorGrammarManagerPrivate(this))
{
    qRegisterMetaType<MessageComposer::PluginGrammarAction>();
}

PluginEditorGrammarManager::~PluginEditorGrammarManager()
{
    delete d;
}

PluginEditorGrammarManager *PluginEditorGrammarManager::self()
{
    static PluginEditorGrammarManager s_self;
    return &s_self;
}

QVector<PimCommon::CustomToolsPlugin *> PluginEditorGrammarManager::pluginsList() const
{
    return d->pluginsList();
}

QString PluginEditorGrammarManager::configGroupName() const
{
    return d->configGroupName();
}

QString PluginEditorGrammarManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QVector<PimCommon::PluginUtilData> PluginEditorGrammarManager::pluginsDataList() const
{
    return d->pluginsDataList();
}

PimCommon::CustomToolsPlugin *PluginEditorGrammarManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
