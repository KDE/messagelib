/*
   SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

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

    ~HeaderStylePluginManager() override;

    [[nodiscard]] QList<MessageViewer::HeaderStylePlugin *> pluginsList() const;
    [[nodiscard]] QStringList pluginListName() const;

    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginsDataList() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] MessageViewer::HeaderStylePlugin *pluginFromIdentifier(const QString &id);

private:
    explicit HeaderStylePluginManager(QObject *parent = nullptr);
    std::unique_ptr<HeaderStylePluginManagerPrivate> const d;
};
}
