/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

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

#include "messageviewerconfiguresettingspluginmanager.h"
#include "messageviewerconfiguresettingsplugin.h"
#include "messageviewer_debug.h"
#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>
#include <QFileInfo>
#include <QSet>

using namespace MessageViewer;

class ConfigureSettingsPluginInfo
{
public:
    ConfigureSettingsPluginInfo()
    {
    }

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    MessageViewer::MessageViewerConfigureSettingsPlugin *plugin = nullptr;
};

class MessageViewer::MessageViewerConfigureSettingsPluginManagerPrivate
{
public:
    MessageViewerConfigureSettingsPluginManagerPrivate(MessageViewerConfigureSettingsPluginManager *qq)
        : q(qq)
    {
    }

    QVector<MessageViewer::MessageViewerConfigureSettingsPlugin *> pluginsList() const;
    QVector<PimCommon::PluginUtilData> pluginDataList() const;
    void initializePluginList();
    void loadPlugin(ConfigureSettingsPluginInfo *item);
    QString configGroupName() const;
    QString configPrefixSettingKey() const;
    MessageViewerConfigureSettingsPlugin *pluginFromIdentifier(const QString &id);
private:
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    QVector<ConfigureSettingsPluginInfo> mPluginList;
    MessageViewerConfigureSettingsPluginManager *q;
};

namespace {
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

QVector<PimCommon::PluginUtilData> MessageViewerConfigureSettingsPluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

QString MessageViewerConfigureSettingsPluginManagerPrivate::configGroupName() const
{
    return QStringLiteral("MessageViewerConfigureSettingsPlugins");
}

QString MessageViewerConfigureSettingsPluginManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("MessageViewerConfigureSettingsPlugin");
}

void MessageViewerConfigureSettingsPluginManagerPrivate::initializePluginList()
{
    const QVector<KPluginMetaData> plugins
        = KPluginLoader::findPlugins(QStringLiteral("messageviewer"), [](
                                         const KPluginMetaData &md) {
        return md.serviceTypes().contains(QLatin1String("MessageViewerConfigureSettingsPlugin/Plugin"));
    });

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(
        configGroupName(), configPrefixSettingKey());
    QSet<QString> unique;
    QVector<int> listOrder;
    while (i.hasPrevious()) {
        ConfigureSettingsPluginInfo info;

        const KPluginMetaData data = i.previous();

        //1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        //2) look at if plugin is activated
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        const QString version = data.version();
        if (pluginVersion() == version) {
            // only load plugins once, even if found multiple times!
            if (unique.contains(info.metaDataFileNameBaseName)) {
                continue;
            }
            const QVariant p
                = data.rawData().value(QStringLiteral("X-KDE-MessageViewer-Configure-Order")).toVariant();
            int order = -1;
            if (p.isValid()) {
                order = p.toInt();
            }
            int pos = 0;
            for (; pos < listOrder.count(); ++pos) {
                if (listOrder.at(pos) > order) {
                    pos--;
                    break;
                }
            }
            pos = qMax(0, pos);
            listOrder.insert(pos, order);
            info.plugin = nullptr;
            mPluginList.insert(pos, info);
            unique.insert(info.metaDataFileNameBaseName);
        } else {
            qCWarning(MESSAGEVIEWER_LOG) << "Plugin " << data.name()
                                         <<
                " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QVector<ConfigureSettingsPluginInfo>::iterator end(mPluginList.end());
    for (QVector<ConfigureSettingsPluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QVector<MessageViewer::MessageViewerConfigureSettingsPlugin *> MessageViewerConfigureSettingsPluginManagerPrivate::pluginsList() const
{
    QVector<MessageViewer::MessageViewerConfigureSettingsPlugin *> lst;
    QVector<ConfigureSettingsPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<ConfigureSettingsPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end;
         ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

void MessageViewerConfigureSettingsPluginManagerPrivate::loadPlugin(ConfigureSettingsPluginInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<MessageViewer::MessageViewerConfigureSettingsPlugin>(q,
                                                                                                           QVariantList()
                                                                                                           << item->metaDataFileNameBaseName);
        //By default it's true
        item->pluginData.mHasConfigureDialog = true;
        mPluginDataList.append(item->pluginData);
    }
}

MessageViewerConfigureSettingsPlugin *MessageViewerConfigureSettingsPluginManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<ConfigureSettingsPluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<ConfigureSettingsPluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

MessageViewerConfigureSettingsPluginManager *MessageViewerConfigureSettingsPluginManager::self()
{
    static MessageViewerConfigureSettingsPluginManager s_self;
    return &s_self;
}

MessageViewerConfigureSettingsPluginManager::MessageViewerConfigureSettingsPluginManager(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerConfigureSettingsPluginManagerPrivate(this))
{
    d->initializePluginList();
}

MessageViewerConfigureSettingsPluginManager::~MessageViewerConfigureSettingsPluginManager()
{
    delete d;
}

QVector<MessageViewer::MessageViewerConfigureSettingsPlugin *> MessageViewerConfigureSettingsPluginManager::pluginsList() const
{
    return d->pluginsList();
}

QString MessageViewerConfigureSettingsPluginManager::configGroupName() const
{
    return d->configGroupName();
}

QString MessageViewerConfigureSettingsPluginManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QVector<PimCommon::PluginUtilData> MessageViewerConfigureSettingsPluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

MessageViewerConfigureSettingsPlugin *MessageViewerConfigureSettingsPluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
