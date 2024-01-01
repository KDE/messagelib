/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditormanager.h"
#include "messagecomposer_debug.h"
#include "plugineditor.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <QFileInfo>
using namespace MessageComposer;

class PluginEditorInfo
{
public:
    PluginEditorInfo() = default;

    KPluginMetaData data;
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
    [[nodiscard]] QList<PluginEditor *> pluginsList() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginDataList() const;
    void initializePlugins();

    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    PluginEditor *pluginFromIdentifier(const QString &id);

private:
    QList<PluginEditorInfo> mPluginList;
    QList<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorManager *const q;
};

void PluginEditorManagerPrivate::initializePlugins()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/kmail/plugineditor"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QListIterator<KPluginMetaData> i(plugins);
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
        info.data = data;
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
    QList<PluginEditorInfo>::iterator end(mPluginList.end());
    for (QList<PluginEditorInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void PluginEditorManagerPrivate::loadPlugin(PluginEditorInfo *item)
{
    if (auto plugin = KPluginFactory::instantiatePlugin<PluginEditor>(item->data, q, QVariantList() << item->metaDataFileName).plugin) {
        item->plugin = plugin;
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        item->plugin->setOrder(item->order);
        mPluginDataList.append(item->pluginData);
    }
}

QList<PluginEditor *> PluginEditorManagerPrivate::pluginsList() const
{
    QList<PluginEditor *> lst;
    QList<PluginEditorInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<PluginEditorInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

QList<PimCommon::PluginUtilData> PluginEditorManagerPrivate::pluginDataList() const
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
    QList<PluginEditorInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<PluginEditorInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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

PluginEditorManager::~PluginEditorManager() = default;

PluginEditorManager *PluginEditorManager::self()
{
    static PluginEditorManager s_self;
    return &s_self;
}

QList<PluginEditor *> PluginEditorManager::pluginsList() const
{
    return d->pluginsList();
}

QList<PimCommon::PluginUtilData> PluginEditorManager::pluginsDataList() const
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

#include "moc_plugineditormanager.cpp"
