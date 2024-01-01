/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditorconverttextmanager.h"
#include "messagecomposer_debug.h"
#include "plugineditorconverttext.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <QFileInfo>

using namespace MessageComposer;

class PluginEditorConvertTextInfo
{
public:
    PluginEditorConvertTextInfo() = default;

    PimCommon::PluginUtilData pluginData;
    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    KPluginMetaData data;
    PluginEditorConvertText *plugin = nullptr;
    bool isEnabled = true;
};

namespace
{
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
    [[nodiscard]] QList<PluginEditorConvertText *> pluginsList() const;
    void initializePlugins();
    QList<PluginEditorConvertTextInfo> mPluginList;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginsDataList() const;
    PluginEditorConvertText *pluginFromIdentifier(const QString &id);

private:
    QList<PimCommon::PluginUtilData> mPluginDataList;
    PluginEditorConvertTextManager *const q;
};

QString PluginEditorConvertTextManagerPrivate::configGroupName() const
{
    return QStringLiteral("KMailPluginEditorConvertText");
}

QString PluginEditorConvertTextManagerPrivate::configPrefixSettingKey() const
{
    return QStringLiteral("PluginEditorConvertText");
}

QList<PimCommon::PluginUtilData> PluginEditorConvertTextManagerPrivate::pluginsDataList() const
{
    return mPluginDataList;
}

void PluginEditorConvertTextManagerPrivate::initializePlugins()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(QStringLiteral("pim6/kmail/plugineditorconverttext"));

    const QPair<QStringList, QStringList> pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        PluginEditorConvertTextInfo info;
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
        if (pluginVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(MESSAGECOMPOSER_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QList<PluginEditorConvertTextInfo>::iterator end(mPluginList.end());
    for (QList<PluginEditorConvertTextInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
}

void PluginEditorConvertTextManagerPrivate::loadPlugin(PluginEditorConvertTextInfo *item)
{
    if (auto plugin = KPluginFactory::instantiatePlugin<PluginEditorConvertText>(item->data, q, QVariantList() << item->metaDataFileName).plugin) {
        item->plugin = plugin;
        item->plugin->setIsEnabled(item->isEnabled);
        item->pluginData.mHasConfigureDialog = item->plugin->hasConfigureDialog();
        mPluginDataList.append(item->pluginData);
    }
}

QList<PluginEditorConvertText *> PluginEditorConvertTextManagerPrivate::pluginsList() const
{
    QList<PluginEditorConvertText *> lst;
    QList<PluginEditorConvertTextInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<PluginEditorConvertTextInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

PluginEditorConvertText *PluginEditorConvertTextManagerPrivate::pluginFromIdentifier(const QString &id)
{
    QList<PluginEditorConvertTextInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<PluginEditorConvertTextInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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

PluginEditorConvertTextManager::~PluginEditorConvertTextManager() = default;

PluginEditorConvertTextManager *PluginEditorConvertTextManager::self()
{
    static PluginEditorConvertTextManager s_self;
    return &s_self;
}

QList<PluginEditorConvertText *> PluginEditorConvertTextManager::pluginsList() const
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

QList<PimCommon::PluginUtilData> PluginEditorConvertTextManager::pluginsDataList() const
{
    return d->pluginsDataList();
}

PluginEditorConvertText *PluginEditorConvertTextManager::pluginFromIdentifier(const QString &id)
{
    return d->pluginFromIdentifier(id);
}

#include "moc_plugineditorconverttextmanager.cpp"
