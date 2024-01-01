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
class ViewerPluginManagerPrivate;
class ViewerPlugin;
/**
 * @brief The ViewerPluginManager class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ViewerPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit ViewerPluginManager(QObject *parent = nullptr);
    ~ViewerPluginManager() override;
    static ViewerPluginManager *self();

    [[nodiscard]] QList<MessageViewer::ViewerPlugin *> pluginsList() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginsDataList() const;

    [[nodiscard]] bool initializePluginList();

    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] MessageViewer::ViewerPlugin *pluginFromIdentifier(const QString &id);
    [[nodiscard]] QString pluginDirectory() const;
    void setPluginDirectory(const QString &directory);
    void setPluginName(const QString &pluginName);
    [[nodiscard]] QString pluginName() const;

private:
    Q_DISABLE_COPY(ViewerPluginManager)
    std::unique_ptr<ViewerPluginManagerPrivate> const d;
};
}
