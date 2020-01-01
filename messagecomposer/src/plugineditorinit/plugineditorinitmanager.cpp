/*
   Copyright (C) 2017-2020 Laurent Montel <montel@kde.org>

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

#include "plugineditorinitmanager.h"
#include "plugineditorinit.h"
#include "messagecomposer_debug.h"
#include <QFileInfo>
#include <QSet>
#include <KPluginLoader>
#include <KPluginMetaData>
#include <KPluginFactory>

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

namespace {
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
    PluginEditorInitManager *q;
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
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail"), [](const KPluginMetaData &md) {
        return md.serviceTypes().contains(QLatin1String("KMailEditor/PluginEditorInit"));
    });

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        PluginEditorInitInfo info;
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
