/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditormanager.h"
#include "messagecomposer_debug.h"
#include "plugineditor.h"

#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>
#include <QFileInfo>
#include <QSet>
using namespace MessageComposer;

class PluginEditorInfo
{
public:
    PluginEditorInfo()
    {
    }

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PluginEditor *plugin = nullptr;
    int order = 0;
    bool isEnabled = true;
};

namespace
{
QString pluginEditorVersion()
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
    Q_REQUIRED_RESULT QVector<PluginEditor *> pluginsList() const;
    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginDataList() const;
    void initializePlugins();

    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    PluginEditor *pluginFromIdentifier(const QString &id);

private:
    QVector<PluginEditorInfo> mPluginList;
    QVector<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorManager *const q;
};

void PluginEditorManagerPrivate::initializePlugins()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("kmail/plugineditor"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        PluginEditorInfo info;
        const KPluginMetaData data = i.previous();

        // 1) get plugin data => name/description etc.
        info.pluginData = PimCommon::PluginUtil::createPluginMetaData(data);
        // 2) look at if plugin is activated
        const bool isPluginActivated =
            PimCommon::PluginUtil::isPluginActivated(pair.first, pair.second, info.pluginData.mEnableByDefault, info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        const QVariant p = data.rawData().value(QStringLiteral("X-KDE-KMailEditor-Order")).toVariant();
        int order = -1;
        if (p.isValid()) {
            order = p.toInt();
        }
        info.order = order;
        if (pluginEditorVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(MESSAGECOMPOSER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QVector<PluginEditorInfo>::iterator end(mPluginList.end());
    for (QVector<PluginEditorInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
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
