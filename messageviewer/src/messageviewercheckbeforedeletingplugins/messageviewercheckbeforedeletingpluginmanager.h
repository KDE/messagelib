/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <PimCommon/PluginUtil>
#include <QObject>
namespace MessageViewer
{
class MessageViewerCheckBeforeDeletingPlugin;
class MessageViewerCheckBeforeDeletingPluginManagerPrivate;
/**
 * @brief The MessageViewerCheckBeforeDeletingPluginManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerCheckBeforeDeletingPluginManager : public QObject
{
    Q_OBJECT
public:
    static MessageViewerCheckBeforeDeletingPluginManager *self();

    explicit MessageViewerCheckBeforeDeletingPluginManager(QObject *parent = nullptr);
    ~MessageViewerCheckBeforeDeletingPluginManager();

    Q_REQUIRED_RESULT QVector<MessageViewer::MessageViewerCheckBeforeDeletingPlugin *> pluginsList() const;

    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT MessageViewerCheckBeforeDeletingPlugin *pluginFromIdentifier(const QString &id);

private:
    MessageViewerCheckBeforeDeletingPluginManagerPrivate *const d;
};
}
