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
        : plugin(Q_NULLPTR)
    {

    }
    QString saveName() const;

    KPluginMetaData metaData;
    MessageViewer::ViewerPlugin *plugin;
};

QString ViewerPluginInfo::saveName() const
{
    return QFileInfo(metaData.fileName()).baseName();
}

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
    QVector<MessageViewer::ViewerPluginManager::ViewerPluginData> pluginDataList() const;

    QString serviceTypeName;
    QString pluginName;
private:
    QVector<ViewerPluginInfo> mPluginList;
    QVector<MessageViewer::ViewerPluginManager::ViewerPluginData> mPluginDataList;
    ViewerPluginManager *q;
};

namespace
{
QString pluginVersion()
{
    return QStringLiteral("2.0");
}
}

bool ViewerPluginManagerPrivate::initializePluginList()
{
    if (serviceTypeName.isEmpty() || pluginName.isEmpty()) {
        return false;
    }
    if (!mPluginList.isEmpty()) {
        return true;
    }

    static const QString s_serviceTypeName = serviceTypeName;
    QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(pluginName, [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(s_serviceTypeName);
    });

    // We need common plugin to avoid to duplicate code between akregator/kmail
    plugins += KPluginLoader::findPlugins(QStringLiteral("messageviewer"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("MessageViewer/ViewerCommonPlugin"));
    });

    KSharedConfigPtr config = KSharedConfig::openConfig(QStringLiteral("pimpluginsrc"));
    QStringList enabledPlugins;
    QStringList disabledPlugins;
    const QString groupPluginName = QStringLiteral("PluginMessageViewer%1").arg(pluginName);
    if (config->hasGroup(groupPluginName)) {
        KConfigGroup grp = config->group(groupPluginName);
        enabledPlugins = grp.readEntry(QStringLiteral("MessageViewerPluginsEnabled"), QStringList());
        disabledPlugins = grp.readEntry(QStringLiteral("MessageViewerPluginsDisabled"), QStringList());
    }
    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        ViewerPluginInfo info;
        info.metaData = i.previous();
        MessageViewer::ViewerPluginManager::ViewerPluginData pluginData;
        pluginData.mDescription = info.metaData.description();
        pluginData.mName = info.metaData.name();
        pluginData.mIdentifier = info.metaData.pluginId();
        pluginData.mEnableByDefault = info.metaData.isEnabledByDefault();
        mPluginDataList.append(pluginData);

        const bool pluginEnabledByUser = enabledPlugins.contains(info.metaData.pluginId());
        const bool pluginDisabledByUser = disabledPlugins.contains(info.metaData.pluginId());
        if ((info.metaData.isEnabledByDefault() && !pluginDisabledByUser)
                || (!info.metaData.isEnabledByDefault() && pluginEnabledByUser)) {
            const QString version = info.metaData.version();
            if (pluginVersion() == version) {
                // only load plugins once, even if found multiple times!
                if (unique.contains(info.saveName())) {
                    continue;
                }
                info.plugin = Q_NULLPTR;
                mPluginList.push_back(info);
                unique.insert(info.saveName());
            } else {
                qCWarning(MESSAGEVIEWER_LOG) << "Plugin name :" << info.metaData.name() << " doesn't have correct plugin version. Please update it";
            }
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
    KPluginLoader pluginLoader(item->metaData.fileName());
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<MessageViewer::ViewerPlugin>(q, QVariantList() << item->saveName());
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

QVector<MessageViewer::ViewerPluginManager::ViewerPluginData> ViewerPluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
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

QVector<MessageViewer::ViewerPluginManager::ViewerPluginData> ViewerPluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}
