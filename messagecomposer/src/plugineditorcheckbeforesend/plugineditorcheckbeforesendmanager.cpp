/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

class PluginEditorCheckBeforeSendManagerInstancePrivate
{
public:
    PluginEditorCheckBeforeSendManagerInstancePrivate()
        : pluginManager(new PluginEditorCheckBeforeSendManager)
    {
    }

    ~PluginEditorCheckBeforeSendManagerInstancePrivate()
    {
        delete pluginManager;
    }
    PluginEditorCheckBeforeSendManager *pluginManager;
};

class PluginEditorCheckBeforeSendInfo
{
public:
    PluginEditorCheckBeforeSendInfo()
        : plugin(Q_NULLPTR)
    {

    }
    QString saveName() const;

    KPluginMetaData metaData;
    PluginEditorCheckBeforeSend *plugin;
};

Q_GLOBAL_STATIC(PluginEditorCheckBeforeSendManagerInstancePrivate, sInstance)

namespace
{
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
    PluginEditorCheckBeforeSendManager *q;
};

bool PluginEditorCheckBeforeSendManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("KMailEditor/PluginCheckBeforeSend"));
    });

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        PluginEditorCheckBeforeSendInfo info;
        info.metaData = i.previous();
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
    QVector<PluginEditorCheckBeforeSendInfo>::iterator end(mPluginList.end());
    for (QVector<PluginEditorCheckBeforeSendInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    return true;
}

void PluginEditorCheckBeforeSendManagerPrivate::loadPlugin(PluginEditorCheckBeforeSendInfo *item)
{
    item->plugin = KPluginLoader(item->metaData.fileName()).factory()->create<PluginEditorCheckBeforeSend>(q, QVariantList() << item->saveName());
}

QVector<PluginEditorCheckBeforeSend *> PluginEditorCheckBeforeSendManagerPrivate::pluginsList() const
{
    QVector<PluginEditorCheckBeforeSend *> lst;
    QVector<PluginEditorCheckBeforeSendInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorCheckBeforeSendInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).plugin) {
            lst << (*it).plugin;
        }
    }
    return lst;
}

PluginEditorCheckBeforeSendManager::PluginEditorCheckBeforeSendManager(QObject *parent)
    : QObject(parent),
      d(new MessageComposer::PluginEditorCheckBeforeSendManagerPrivate(this))
{
}

PluginEditorCheckBeforeSendManager::~PluginEditorCheckBeforeSendManager()
{
    delete d;
}

PluginEditorCheckBeforeSendManager *PluginEditorCheckBeforeSendManager::self()
{
    return sInstance->pluginManager;
}

QString PluginEditorCheckBeforeSendInfo::saveName() const
{
    return QFileInfo(metaData.fileName()).baseName();
}

QVector<PluginEditorCheckBeforeSend *> PluginEditorCheckBeforeSendManager::pluginsList() const
{
    return d->pluginsList();
}
