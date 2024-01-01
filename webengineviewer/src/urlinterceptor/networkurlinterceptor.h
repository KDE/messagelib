/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QWebEngineUrlRequestInterceptor>
#include <memory>
namespace WebEngineViewer
{
class NetworkUrlInterceptorPrivate;
class NetworkPluginUrlInterceptorInterface;
/**
 * @brief The NetworkUrlInterceptor class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT
public:
    explicit NetworkUrlInterceptor(QObject *parent = nullptr);
    ~NetworkUrlInterceptor() override;

    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

    void addInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);
    void removeInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);

private:
    std::unique_ptr<NetworkUrlInterceptorPrivate> const d;
};
}
