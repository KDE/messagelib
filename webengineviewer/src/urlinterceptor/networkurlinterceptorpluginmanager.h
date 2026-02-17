/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QList>
#include <QObject>
#include <TextAddonsWidgets/PluginUtil>
namespace WebEngineViewer
{
class NetworkUrlInterceptorPluginManagerPrivate;
class NetworkPluginUrlInterceptor;
/*!
 * \class WebEngineViewer::NetworkUrlInterceptorPluginManager
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/NetworkUrlInterceptorPluginManager
 *
 * \brief The NetworkUrlInterceptorPluginManager class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptorPluginManager : public QObject
{
    Q_OBJECT
public:
    /*! Returns the singleton instance. */
    static NetworkUrlInterceptorPluginManager *self();
    /*! Constructs a NetworkUrlInterceptorPluginManager with the given parent. */
    explicit NetworkUrlInterceptorPluginManager(QObject *parent = nullptr);
    /*! Destroys the NetworkUrlInterceptorPluginManager object. */
    ~NetworkUrlInterceptorPluginManager() override;

    /*! Returns the list of plugins. */
    [[nodiscard]] QList<WebEngineViewer::NetworkPluginUrlInterceptor *> pluginsList() const;
    /*! Returns the configuration group name. */
    [[nodiscard]] QString configGroupName() const;
    /*! Returns the configuration prefix setting key. */
    [[nodiscard]] QString configPrefixSettingKey() const;
    /*! Returns the list of plugin utility data. */
    [[nodiscard]] QList<TextAddonsWidgets::PluginUtilData> pluginsDataList() const;
    /*! Returns the plugin with the specified identifier. */
    [[nodiscard]] WebEngineViewer::NetworkPluginUrlInterceptor *pluginFromIdentifier(const QString &id);

private:
    std::unique_ptr<NetworkUrlInterceptorPluginManagerPrivate> const d;
};
}
