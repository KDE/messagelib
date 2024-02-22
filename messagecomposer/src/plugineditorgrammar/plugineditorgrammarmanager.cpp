/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorgrammarmanager.h"
#include "messagecomposer_debug.h"
#include "plugineditorgrammarcustomtoolsviewinterface.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <PimCommon/CustomToolsPlugin>
#include <QFileInfo>

using namespace MessageComposer;

class PluginEditorGrammarInfo
{
public:
    PluginEditorGrammarInfo() = default;

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    KPluginMetaData data;
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
    [[nodiscard]] QList<PimCommon::CustomToolsPlugin *> pluginsList() const;
    void initializePlugins();
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginsDataList() const;

    QList<PluginEditorGrammarInfo> mPluginList;
    PimCommon::CustomToolsPlugin *pluginFromIdentifier(const QString &id);

private:
    QList<PimCommon::PluginUtilData> mPluginDataList;
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

QList<PimCommon::PluginUtilData> PluginEditorGrammarManagerPrivate::pluginsDataList() const
{
    return mPluginDataList;
}

void PluginEditorGrammarManagerPrivate::initializePlugins()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/kmail/plugineditorgrammar"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QListIterator<KPluginMetaData> i(plugins);
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
        info.data = data;
        if (pluginEditorGrammarVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(MESSAGECOMPOSER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QList<PluginEditorGrammarInfo>::iterator end(mPluginList.end());
    for (QList<PluginEditorGrammarInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void PluginEditorGrammarManagerPrivate::loadPlugin(PluginEditorGrammarInfo *item)
{
    if (auto plugin = KPluginFactory::instantiatePlugin<PimCommon::CustomToolsPlugin>(item->data, q, QVariantList() << item->metaDataFileName).plugin) {
        item->plugin = plugin;
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QList<PimCommon::CustomToolsPlugin *> PluginEditorGrammarManagerPrivate::pluginsList() const
{
    QList<PimCommon::CustomToolsPlugin *> lst;
    QList<PluginEditorGrammarInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<PluginEditorGrammarInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

PimCommon::CustomToolsPlugin *PluginEditorGrammarManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QList<PluginEditorGrammarInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<PluginEditorGrammarInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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

PluginEditorGrammarManager::~PluginEditorGrammarManager() = default;

PluginEditorGrammarManager *PluginEditorGrammarManager::self()
{
    static PluginEditorGrammarManager s_self;
    return &s_self;
}

QList<PimCommon::CustomToolsPlugin *> PluginEditorGrammarManager::pluginsList() const
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

QList<PimCommon::PluginUtilData> PluginEditorGrammarManager::pluginsDataList() const
{
    return d->pluginsDataList();
}

PimCommon::CustomToolsPlugin *PluginEditorGrammarManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

#include "moc_plugineditorgrammarmanager.cpp"
