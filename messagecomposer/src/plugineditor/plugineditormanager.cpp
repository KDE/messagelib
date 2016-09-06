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

#include "plugineditor.h"
#include "messagecomposer_debug.h"
#include "plugineditormanager.h"

#include <QFileInfo>
#include <QSet>
#include <KPluginLoader>
#include <kpluginmetadata.h>
#include <KPluginFactory>
using namespace MessageComposer;

class PluginEditorManagerInstancePrivate
{
public:
    PluginEditorManagerInstancePrivate()
        : pluginManager(new PluginEditorManager)
    {
    }

    ~PluginEditorManagerInstancePrivate()
    {
        delete pluginManager;
    }
    PluginEditorManager *pluginManager;
};

class PluginEditorInfo
{
public:
    PluginEditorInfo()
        : order(0),
          plugin(Q_NULLPTR)
    {

    }
    QString saveName() const;

    KPluginMetaData metaData;
    int order;
    PluginEditor *plugin;
};

Q_GLOBAL_STATIC(PluginEditorManagerInstancePrivate, sInstance)

namespace
{
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
    bool initializePlugins();
    QVector<PluginEditorInfo> mPluginList;
    PluginEditorManager *q;
};

bool PluginEditorManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("KMailEditor/Plugin"));
    });

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        PluginEditorInfo info;
        info.metaData = i.previous();
        if (info.metaData.isEnabledByDefault()) {
            const QVariant p = info.metaData.rawData().value(QStringLiteral("X-KDE-KMailEditor-Order")).toVariant();
            int order = -1;
            if (p.isValid()) {
                order = p.toInt();
            }
            info.order = order;
            if (pluginVersion() == info.metaData.version()) {
                // only load plugins once, even if found multiple times!
                if (unique.contains(info.saveName())) {
                    continue;
                }
                info.plugin = Q_NULLPTR;
                mPluginList.push_back(info);
                unique.insert(info.saveName());
            } else {
                qCWarning(MESSAGECOMPOSER_LOG) << "Plugin " << info.metaData.name() << " doesn't have correction plugin version. It will not be loaded.";
            }
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
    KPluginLoader pluginLoader(item->metaData.fileName());
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<PluginEditor>(q, QVariantList() << item->saveName());
        item->plugin->setOrder(item->order);
    }
}

QVector<PluginEditor *> PluginEditorManagerPrivate::pluginsList() const
{
    QVector<PluginEditor *> lst;
    QVector<PluginEditorInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).plugin) {
            lst << (*it).plugin;
        }
    }
    return lst;
}

PluginEditorManager::PluginEditorManager(QObject *parent)
    : QObject(parent),
      d(new MessageComposer::PluginEditorManagerPrivate(this))
{
}

PluginEditorManager::~PluginEditorManager()
{
    delete d;
}

PluginEditorManager *PluginEditorManager::self()
{
    return sInstance->pluginManager;
}

QString PluginEditorInfo::saveName() const
{
    return QFileInfo(metaData.fileName()).baseName();
}

QVector<PluginEditor *> PluginEditorManager::pluginsList() const
{
    return d->pluginsList();
}
