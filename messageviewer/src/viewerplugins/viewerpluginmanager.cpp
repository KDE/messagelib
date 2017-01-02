/*
   Copyright (C) 2015-2016 Laurent Montel <montel@kde.org>

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

#include "viewerpluginmanager.h"
#include "viewerplugin.h"
#include "messageviewer_debug.h"

#include <KSharedConfig>
#include <kpluginmetadata.h>
#include <KPluginLoader>
#include <KPluginFactory>
#include <QFileInfo>
#include <QSet>
#include <KConfigGroup>

using namespace MessageViewer;

class ViewerPluginManagerPrivateInstancePrivate
{
public:
    ViewerPluginManagerPrivateInstancePrivate()
        : viewerPluginManager(new ViewerPluginManager)
    {
    }

    ~ViewerPluginManagerPrivateInstancePrivate()
    {
        delete viewerPluginManager;
    }

    ViewerPluginManager *viewerPluginManager;
};

Q_GLOBAL_STATIC(ViewerPluginManagerPrivateInstancePrivate, sInstance)

class ViewerPluginInfo
{
public:
    ViewerPluginInfo()
        : plugin(nullptr),
          isEnabled(false)
    {

    }

    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PimCommon::PluginUtilData pluginData;
    MessageViewer::ViewerPlugin *plugin;
    bool isEnabled;
};

class MessageViewer::ViewerPluginManagerPrivate
{
public:
    ViewerPluginManagerPrivate(ViewerPluginManager *qq)
        : q(qq)
    {

    }
    bool initializePluginList();
    void loadPlugin(ViewerPluginInfo *item);
    QVector<MessageViewer::ViewerPlugin *> pluginsList() const;
    QVector<PimCommon::PluginUtilData> pluginDataList() const;

    QString serviceTypeName;
    QString pluginName;
    QString configGroupName() const;
    QString configPrefixSettingKey() const;
    ViewerPlugin *pluginFromIdentifier(const QString &id);
private:
    QVector<ViewerPluginInfo> mPluginList;
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    ViewerPluginManager *q;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("2.0");
}
}

QString ViewerPluginManagerPrivate::configGroupName() const
{
    return QStringLiteral("PluginMessageViewer%1").arg(pluginName);
}

QString ViewerPluginManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("MessageViewerPlugins");
}

bool ViewerPluginManagerPrivate::initializePluginList()
{
    if (!mPluginList.isEmpty()) {
        return true;
    }
    if (serviceTypeName.isEmpty() || pluginName.isEmpty()) {
        return false;
    }

    static const QString s_serviceTypeName = serviceTypeName;
    QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(pluginName, [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(s_serviceTypeName);
    });

    // We need common plugin to avoid to duplicate code between akregator/kmail
    plugins += KPluginLoader::findPlugins(QStringLiteral("messageviewer"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("MessageViewer/ViewerCommonPlugin"));
    });

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());
    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        ViewerPluginInfo info;

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
            qCWarning(MESSAGEVIEWER_LOG) << "Plugin name :" << data.name() << " doesn't have correct plugin version. Please update it";
        }
    }
    QVector<ViewerPluginInfo>::iterator end(mPluginList.end());
    for (QVector<ViewerPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    return true;
}

void ViewerPluginManagerPrivate::loadPlugin(ViewerPluginInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<MessageViewer::ViewerPlugin>(q, QVariantList() << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QVector<ViewerPlugin *> ViewerPluginManagerPrivate::pluginsList() const
{
    QVector<MessageViewer::ViewerPlugin *> lst;
    QVector<ViewerPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<ViewerPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).plugin) {
            lst << (*it).plugin;
        }
    }
    return lst;
}

QVector<PimCommon::PluginUtilData> ViewerPluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

ViewerPlugin *ViewerPluginManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<ViewerPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<ViewerPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

ViewerPluginManager::ViewerPluginManager(QObject *parent)
    : QObject(parent),
      d(new MessageViewer::ViewerPluginManagerPrivate(this))
{
}

MessageViewer::ViewerPluginManager::~ViewerPluginManager()
{
    delete d;
}

bool ViewerPluginManager::initializePluginList()
{
    return d->initializePluginList();
}

ViewerPluginManager *ViewerPluginManager::self()
{
    return sInstance->viewerPluginManager;
}

QVector<MessageViewer::ViewerPlugin *> ViewerPluginManager::pluginsList() const
{
    return d->pluginsList();
}

void ViewerPluginManager::setServiceTypeName(const QString &serviceName)
{
    d->serviceTypeName = serviceName;
}

QString ViewerPluginManager::serviceTypeName() const
{
    return d->serviceTypeName;
}

void ViewerPluginManager::setPluginName(const QString &pluginName)
{
    d->pluginName = pluginName;
}

QString ViewerPluginManager::pluginName() const
{
    return d->pluginName;
}

QVector<PimCommon::PluginUtilData> ViewerPluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

QString ViewerPluginManager::configGroupName() const
{
    return d->configGroupName();
}

QString ViewerPluginManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

MessageViewer::ViewerPlugin *ViewerPluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

