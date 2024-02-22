/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

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

    ~MessageViewerConfigureSettingsPluginManager() override;

    [[nodiscard]] QList<MessageViewer::MessageViewerConfigureSettingsPlugin *> pluginsList() const;

    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginsDataList() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] MessageViewerConfigureSettingsPlugin *pluginFromIdentifier(const QString &id);

private:
    explicit MessageViewerConfigureSettingsPluginManager(QObject *parent = nullptr);
    std::unique_ptr<MessageViewerConfigureSettingsPluginManagerPrivate> const d;
};
}
