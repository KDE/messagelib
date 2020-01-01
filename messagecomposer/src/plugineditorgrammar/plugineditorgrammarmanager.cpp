/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "plugineditorgrammarmanager.h"
#include <PimCommon/CustomToolsPlugin>
#include "messagecomposer_debug.h"
#include "plugineditorgrammarcustomtoolsviewinterface.h"
#include <QFileInfo>
#include <QSet>
#include <KPluginLoader>
#include <KPluginMetaData>
#include <KPluginFactory>

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

namespace {
QString pluginVersion()
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
    PluginEditorGrammarManager *q;
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
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail"), [](const KPluginMetaData &md) {
        return md.serviceTypes().contains(QLatin1String("KMailEditor/PluginEditorGrammar"));
    });

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        PluginEditorGrammarInfo info;
        const KPluginMetaData data = i.previous();

        //1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        //2) look at if plugin is activated
        const bool isPluginActivated = PimCommon::PluginUtil::isPluginActivated(pair.first, pair.second, info.pluginData.mEnableByDefault, info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        if (pluginVersion() == data.version()) {
            // only load plugins once, even if found multiple times!
            if (unique.contains(info.metaDataFileNameBaseName)) {
                continue;
            }
            info.plugin = nullptr;
            mPluginList.push_back(info);
            unique.insert(info.metaDataFileNameBaseName);
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
