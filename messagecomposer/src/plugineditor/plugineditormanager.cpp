/*
   SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugineditormanager.h"
using namespace Qt::Literals::StringLiterals;

#include "messagecomposer_debug.h"
#include "plugineditor.h"
#include <KPluginFactory>
#include <KPluginMetaData>
#include <PimCommon/PluginUtil>
#include <QFileInfo>
using namespace MessageComposer;

class PluginEditorInfo
{
public:
    PluginEditorInfo() = default;

    KPluginMetaData data;
    TextAddonsWidgets::PluginUtilData pluginData;
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
    return u"1.0"_s;
}
}

class MessageComposer::PluginEditorManagerPrivate
{
public:
    explicit PluginEditorManagerPrivate(PluginEditorManager *qq)
        : q(qq)
    {
        initializePlugins();
    }

    void loadPlugin(PluginEditorInfo *item);
    [[nodiscard]] QList<PluginEditor *> pluginsList() const;
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginDataList() const;
    void initializePlugins();

    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    PluginEditor *pluginFromIdentifier(const QString &id);

private:
    QList<PluginEditorInfo> mPluginList;
    QList<TextAddonsWidgets::PluginUtilData> mPluginDataList;
    PluginEditorManager *const q;
};

void PluginEditorManagerPrivate::initializePlugins()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(u"pim6/kmail/plugineditor"_s);

    const TextAddonsWidgets::PluginUtil::PluginsStateList pair = PimCommon::PluginUtil::loadPluginSetting(configGroupName(), configPrefixSettingKey());

    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        PluginEditorInfo info;
        const KPluginMetaData data = i.previous();

        // 1) get plugin data => name/description etc.
        info.pluginData = TextAddonsWidgets::PluginUtil::createPluginMetaData(data);
        // 2) look at if plugin is activated
        const bool isPluginActivated = TextAddonsWidgets::PluginUtil::isPluginActivated(pair.enabledPluginList,
                                                                                        pair.disabledPluginList,
                                                                                        info.pluginData.mEnableByDefault,
                                                                                        info.pluginData.mIdentifier);
        info.isEnabled = isPluginActivated;
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        info.data = data;
        const QVariant p = data.rawData().value(u"X-KDE-KMailEditor-Order"_s).toVariant();
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

QList<TextAddonsWidgets::PluginUtilData> PluginEditorManagerPrivate::pluginDataList() const
{
    return mPluginDataList;
}

QString PluginEditorManagerPrivate::configGroupName() const
{
    return u"KMailPluginEditor"_s;
}

QString PluginEditorManagerPrivate::configPrefixSettingKey() const
{
    return u"KMailEditorPlugin"_s;
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

QList<TextAddonsWidgets::PluginUtilData> PluginEditorManager::pluginsDataList() const
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
