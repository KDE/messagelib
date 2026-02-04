/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
/*!
 * \class WebEngineViewer::NetworkUrlInterceptor
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/NetworkUrlInterceptor
 *
 * \brief The NetworkUrlInterceptor class
 * \author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkUrlInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT
public:
    /*! Constructs a NetworkUrlInterceptor object. */
    explicit NetworkUrlInterceptor(QObject *parent = nullptr);
    /*! Destroys the NetworkUrlInterceptor object. */
    ~NetworkUrlInterceptor() override;

    /*! Intercepts URL requests. */
    void interceptRequest(QWebEngineUrlRequestInfo &info) override;

    /*! Adds an interceptor. */
    void addInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);
    /*! Removes an interceptor. */
    void removeInterceptor(NetworkPluginUrlInterceptorInterface *interceptor);

private:
    std::unique_ptr<NetworkUrlInterceptorPrivate> const d;
};
}
