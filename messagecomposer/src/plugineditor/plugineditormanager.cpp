/*
   Copyright (C) 2015-2018 Laurent Montel <montel@kde.org>

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

#include "plugineditor.h"
#include "messagecomposer_debug.h"
#include "plugineditormanager.h"
#include <PimCommon/PluginUtil>

#include <QFileInfo>
#include <QSet>
#include <KPluginLoader>
#include <kpluginmetadata.h>
#include <KPluginFactory>
using namespace MessageComposer;

class PluginEditorInfo
{
public:
    PluginEditorInfo()
    {
    }

    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PimCommon::PluginUtilData pluginData;
    int order = 0;
    PluginEditor *plugin = nullptr;
    bool isEnabled = true;
};

namespace {
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

class MessageComposer::PluginEditorManagerPrivate
{
public:
    PluginEditorManagerPrivate(PluginEditorManager *qq)
        : q(qq)
    {
        initializePlugins();
    }

    void loadPlugin(PluginEditorInfo *item);
    QVector<PluginEditor *> pluginsList() const;
    QVector<PimCommon::PluginUtilData> pluginDataList() const;
    bool initializePlugins();

    QString configGroupName() const;
    QString configPrefixSettingKey() const;
    PluginEditor *pluginFromIdentifier(const QString &id);
private:
    QVector<PluginEditorInfo> mPluginList;
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorManager *q;
};

bool PluginEditorManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail"), [](const KPluginMetaData &md) {
        return md.serviceTypes().contains(QStringLiteral("KMailEditor/Plugin"));
    });

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        PluginEditorInfo info;
        const KPluginMetaData data = i.previous();

        //1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        //2) look at if plugin is activated
        const bool isPluginActivated = PimCommon::PluginUtil::isPluginActivated(pair.first, pair.second, info.pluginData.mEnableByDefault, info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        const QVariant p = data.rawData().value(QStringLiteral("X-KDE-KMailEditor-Order")).toVariant();
        int order = -1;
        if (p.isValid()) {
            order = p.toInt();
        }
        info.order = order;
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
    QVector<PluginEditorInfo>::iterator end(mPluginList.end());
    for (QVector<PluginEditorInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    return true;
}

void PluginEditorManagerPrivate::loadPlugin(PluginEditorInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<PluginEditor>(q, QVariantList() << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        item->plugin->setOrder(item->order);
        mPluginDataList.append(item->pluginData);
    }
}

QVector<PluginEditor *> PluginEditorManagerPrivate::pluginsList() const
{
    QVector<PluginEditor *> lst;
    QVector<PluginEditorInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

QVector<PimCommon::PluginUtilData> PluginEditorManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

QString PluginEditorManagerPrivate::configGroupName() const
{
    return QStringLiteral("KMailPluginEditor");
}

QString PluginEditorManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("KMailEditorPlugin");
}

PluginEditor *PluginEditorManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<PluginEditorInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

PluginEditorManager::PluginEditorManager(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorManagerPrivate(this))
{
}

PluginEditorManager::~PluginEditorManager()
{
    delete d;
}

PluginEditorManager *PluginEditorManager::self()
{
    static PluginEditorManager s_self;
    return &s_self;
}

QVector<PluginEditor *> PluginEditorManager::pluginsList() const
{
    return d->pluginsList();
}

QVector<PimCommon::PluginUtilData> PluginEditorManager::pluginsDataList() const
{
    return d->pluginDataList();
}

QString PluginEditorManager::configGroupName() const
{
    return d->configGroupName();
}

QString PluginEditorManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

PluginEditor *PluginEditorManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
