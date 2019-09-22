/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

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

#include "plugineditorconverttextmanager.h"
#include "plugineditorconverttext.h"
#include "messagecomposer_debug.h"
#include <QFileInfo>
#include <QSet>
#include <KPluginLoader>
#include <kpluginmetadata.h>
#include <KPluginFactory>

using namespace MessageComposer;

class PluginEditorConvertTextInfo
{
public:
    PluginEditorConvertTextInfo()
    {
    }

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PluginEditorConvertText *plugin = nullptr;
    bool isEnabled = true;
};

namespace {
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

class MessageComposer::PluginEditorConvertTextManagerPrivate
{
public:
    PluginEditorConvertTextManagerPrivate(PluginEditorConvertTextManager *qq)
        : q(qq)
    {
        initializePlugins();
    }

    void loadPlugin(PluginEditorConvertTextInfo *item);
    Q_REQUIRED_RESULT QVector<PluginEditorConvertText *> pluginsList() const;
    void initializePlugins();
    QVector<PluginEditorConvertTextInfo> mPluginList;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    PluginEditorConvertText *pluginFromIdentifier(const QString &id);
private:
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorConvertTextManager *q;
};

QString PluginEditorConvertTextManagerPrivate::configGroupName() const
{
    return QStringLiteral("KMailPluginEditorConvertText");
}

QString PluginEditorConvertTextManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("PluginEditorConvertText");
}

QVector<PimCommon::PluginUtilData> PluginEditorConvertTextManagerPrivate::pluginsDataList() const
{
    return mPluginDataList;
}

void PluginEditorConvertTextManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail"), [](const KPluginMetaData &md) {
        return md.serviceTypes().contains(QLatin1String("KMailEditor/PluginEditorConvertText"));
    });

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        PluginEditorConvertTextInfo info;
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
    QVector<PluginEditorConvertTextInfo>::iterator end(mPluginList.end());
    for (QVector<PluginEditorConvertTextInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void PluginEditorConvertTextManagerPrivate::loadPlugin(PluginEditorConvertTextInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<PluginEditorConvertText>(q, QVariantList() << item->metaDataFileNameBaseName);
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QVector<PluginEditorConvertText *> PluginEditorConvertTextManagerPrivate::pluginsList() const
{
    QVector<PluginEditorConvertText *> lst;
    QVector<PluginEditorConvertTextInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorConvertTextInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

PluginEditorConvertText *PluginEditorConvertTextManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QVector<PluginEditorConvertTextInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<PluginEditorConvertTextInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).pluginData.mIdentifier == id) {
            return (*it).plugin;
        }
    }
    return {};
}

PluginEditorConvertTextManager::PluginEditorConvertTextManager(QObject *parent)
    : QObject(parent)
    , d(new MessageComposer::PluginEditorConvertTextManagerPrivate(this))
{
}

PluginEditorConvertTextManager::~PluginEditorConvertTextManager()
{
    delete d;
}

PluginEditorConvertTextManager *PluginEditorConvertTextManager::self()
{
    static PluginEditorConvertTextManager s_self;
    return &s_self;
}

QVector<PluginEditorConvertText *> PluginEditorConvertTextManager::pluginsList() const
{
    return d->pluginsList();
}

QString PluginEditorConvertTextManager::configGroupName() const
{
    return d->configGroupName();
}

QString PluginEditorConvertTextManager::configPrefixSettingKey() const
{
    return d->configPrefixSettingKey();
}

QVector<PimCommon::PluginUtilData> PluginEditorConvertTextManager::pluginsDataList() const
{
    return d->pluginsDataList();
}

PluginEditorConvertText *PluginEditorConvertTextManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}
