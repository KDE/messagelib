/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <PimCommon/PluginUtil>
#include <QList>
#include <QObject>
namespace WebEngineViewer
{
class NetworkUrlInterceptorPluginManagerPrivate;
class NetworkPluginUrlInterceptor;
/**
 * @brief The NetworkUrlInterceptorPluginManager class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptorPluginManager : public QObject
{
    Q_OBJECT
public:
    static NetworkUrlInterceptorPluginManager *self();
    explicit NetworkUrlInterceptorPluginManager(QObject *parent = nullptr);
    ~NetworkUrlInterceptorPluginManager() override;

    [[nodiscard]] QList<WebEngineViewer::NetworkPluginUrlInterceptor *> pluginsList() const;
    [[nodiscard]] QString configGroupName() const;
    [[nodiscard]] QString configPrefixSettingKey() const;
    [[nodiscard]] QList<PimCommon::PluginUtilData> pluginsDataList() const;
    [[nodiscard]] WebEngineViewer::NetworkPluginUrlInterceptor *pluginFromIdentifier(const QString &id);

private:
    std::unique_ptr<NetworkUrlInterceptorPluginManagerPrivate> const d;
};
}
