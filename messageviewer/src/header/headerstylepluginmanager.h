/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <PimCommon/PluginUtil>
#include <QObject>
namespace MessageViewer
{
class HeaderStylePlugin;
class HeaderStylePluginManagerPrivate;
/**
 * @brief The HeaderStylePluginManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT HeaderStylePluginManager : public QObject
{
    Q_OBJECT
public:
    static HeaderStylePluginManager *self();

    explicit HeaderStylePluginManager(QObject *parent = nullptr);
    ~HeaderStylePluginManager() override;

    Q_REQUIRED_RESULT QVector<MessageViewer::HeaderStylePlugin *> pluginsList() const;
    Q_REQUIRED_RESULT QStringList pluginListName() const;

    Q_REQUIRED_RESULT QVector<PimCommon::PluginUtilData> pluginsDataList() const;
    Q_REQUIRED_RESULT QString configPrefixSettingKey() const;
    Q_REQUIRED_RESULT QString configGroupName() const;
    Q_REQUIRED_RESULT MessageViewer::HeaderStylePlugin *pluginFromIdentifier(const QString &id);

private:
    HeaderStylePluginManagerPrivate *const d;
};
}
