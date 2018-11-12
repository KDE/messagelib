/*
   Copyright (C) 2018 Laurent Montel <montel@kde.org>

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

#include "messageviewerconfigurepluginmanager.h"
#include "headerstyleplugin.h"
#include "messageviewer_debug.h"
#include <KPluginFactory>
#include <KPluginLoader>
#include <kpluginmetadata.h>
#include <QFileInfo>
#include <QSet>

using namespace MessageViewer;

class HeaderStylePluginInfo
{
public:
    HeaderStylePluginInfo()
    {
    }

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    MessageViewer::HeaderStylePlugin *plugin = nullptr;
    bool isEnabled = false;
};

class MessageViewer::MessageViewerConfigurePluginManagerPrivate
{
public:
    MessageViewerConfigurePluginManagerPrivate(MessageViewerConfigurePluginManager *qq)
        : q(qq)
    {
    }

    QVector<MessageViewer::HeaderStylePlugin *> pluginsList() const;
    QVector<PimCommon::PluginUtilData> pluginDataList() const;
    void initializePluginList();
    void loadPlugin(HeaderStylePluginInfo *item);
    QString configGroupName() const;
    QString configPrefixSettingKey() const;
    MessageViewer::HeaderStylePlugin *pluginFromIdentifier(const QString &id);
private:
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    QVector<HeaderStylePluginInfo> mPluginList;
    MessageViewerConfigurePluginManager *q;
};

namespace {
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

QVector<PimCommon::PluginUtilData> MessageViewerConfigurePluginManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

QString MessageViewerConfigurePluginManagerPrivate::configGroupName() const
{
    return QStringLiteral("MessageViewerConfigurePlugins");
}

QString MessageViewerConfigurePluginManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("MessageViewerConfigurePlugin");
}

void MessageViewerConfigurePluginManagerPrivate::initializePluginList()
{
    const QVector<KPluginMetaData> plugins
        = KPluginLoader::findPlugins(QStringLiteral("messageviewer"), [](
                                         const KPluginMetaData &md) {
        return md.serviceTypes().contains(QLatin1String("MessageViewerConfigurePlugin/Plugin"));
    });

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(
        configGroupName(), configPrefixSettingKey());
    QSet<QString> unique;
    QVector<int> listOrder;
    while (i.hasPrevious()) {
        HeaderStylePluginInfo info;

        const KPluginMetaData data = i.previous();

        //1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        //2) look at if plugin is activated
        const bool isPluginActivated = PimCommon::PluginUtil::isPluginActivated(pair.first,
                                                                                pair.second,
                                                                                info.pluginData.mEnableByDefault,
                                                                                info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
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
    QVector<HeaderStylePluginInfo>::iterator end(mPluginList.end());
    for (QVector<HeaderStylePluginInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

QVector<MessageViewer::HeaderStylePlugin *> MessageViewerConfigurePluginManagerPrivate::pluginsList() const
{
    QVector<MessageViewer::HeaderStylePlugin *> lst;
    QVector<HeaderStylePluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<HeaderStylePluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end;
         ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

void MessageViewerConfigurePluginManagerPrivate::loadPlugin(HeaderStylePluginInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<MessageViewer::HeaderStylePlugin>(q,
                                                                                        QVariantList()
                                                                                        << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = false;
        mPluginDataList.append(item->pluginData);
    }
}

MessageViewer::HeaderStylePlugin *MessageViewerConfigurePluginManagerPrivate::pluginFromIdentifier(
    const QString &id)
{
    QVector<HeaderStylePluginInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<HeaderStylePluginInfo>::ConstIterator it = mPluginList.constBegin(); it != end;
         ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

MessageViewerConfigurePluginManager *MessageViewerConfigurePluginManager::self()
{
    static MessageViewerConfigurePluginManager s_self;
    return &s_self;
}

MessageViewerConfigurePluginManager::MessageViewerConfigurePluginManager(QObject *parent)
    : QObject(parent)
    , d(new MessageViewer::MessageViewerConfigurePluginManagerPrivate(this))
{
    d->initializePluginList();
}

MessageViewerConfigurePluginManager::~MessageViewerConfigurePluginManager()
{
    delete d;
}

QVector<MessageViewer::HeaderStylePlugin *> MessageViewerConfigurePluginManager::pluginsList() const
{
    return d->pluginsList();
}

QStringList MessageViewerConfigurePluginManager::pluginListName() const
{
    QStringList lst;
    lst.reserve(d->pluginsList().count());
    for (MessageViewer::HeaderStylePlugin *plugin : d->pluginsList()) {
        lst << plugin->name();
    }
    return lst;
}

QString MessageViewerConfigurePluginManager::configGroupName() const
{
    return d->configGroupName();
}

QString MessageViewerConfigurePluginManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QVector<PimCommon::PluginUtilData> MessageViewerConfigurePluginManager::pluginsDataList() const
{
    return d->pluginDataList();
}

MessageViewer::HeaderStylePlugin *MessageViewerConfigurePluginManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
