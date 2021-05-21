/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <PimCommon/PluginUtil>
#include <QObject>
namespace MessageViewer
{
class MessageViewerConfigureSettingsPlugin;
class MessageViewerConfigureSettingsPluginManagerPrivate;
/**
 * @brief The MessageViewerConfigureSettingsPluginManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerConfigureSettingsPluginManager : public QObject
{
    Q_OBJECT
public:
    static MessageViewerConfigureSettingsPluginManager *self();

    explicit MessageViewerConfigureSettingsPluginManager(QObject *parent = nullptr);
    ~MessageViewerConfigureSettingsPluginManager();

    Q_REQUIRED_RESULT QVector<MessageViewer::MessageViewerConfigureSettingsPlugin *> pluginsList() const;

    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT MessageViewerConfigureSettingsPlugin *pluginFromIdentifier(const QString &id);

private:
    MessageViewerConfigureSettingsPluginManagerPrivate *const d;
};
}
