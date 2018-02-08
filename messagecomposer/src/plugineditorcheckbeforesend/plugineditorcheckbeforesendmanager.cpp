/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

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

#include "plugineditorcheckbeforesendmanager.h"
#include "plugineditorcheckbeforesend.h"
#include "messagecomposer_debug.h"
#include <QFileInfo>
#include <QSet>
#include <KPluginLoader>
#include <kpluginmetadata.h>
#include <KPluginFactory>

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

namespace {
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
    QVector<PluginEditorCheckBeforeSend *> pluginsList() const;
    bool initializePlugins();
    QVector<PluginEditorCheckBeforeSendInfo> mPluginList;
    QString configPrefixSettingKey() const;
    QString configGroupName() const;
    QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    PluginEditorCheckBeforeSend *pluginFromIdentifier(const QString &id);
private:
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorCheckBeforeSendManager *q;
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

bool PluginEditorCheckBeforeSendManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail"), [](const KPluginMetaData &md) {
        return md.serviceTypes().contains(QLatin1String("KMailEditor/PluginCheckBeforeSend"));
    });

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        PluginEditorCheckBeforeSendInfo info;
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
    QVector<PluginEditorCheckBeforeSendInfo>::iterator end(mPluginList.end());
    for (QVector<PluginEditorCheckBeforeSendInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    return true;
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
