/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "webengineviewer_export.h"
#include <QUrl>
#include <QWebEngineUrlRequestInfo>
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
namespace WebEngineViewer
{
class WEBENGINEVIEWER_EXPORT BlockExternalResourcesUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    /*! Constructs a BlockExternalResourcesUrlInterceptor with the given parent. */
    explicit BlockExternalResourcesUrlInterceptor(QObject *parent = nullptr);
    /*! Destroys the BlockExternalResourcesUrlInterceptor object. */
    ~BlockExternalResourcesUrlInterceptor() override;

    /*! Intercepts the URL request. */
    [[nodiscard]] bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    /*! Intercepts a request for the given URL, resource type, and navigation type. Used by autotest only. */
    [[nodiscard]] bool
    interceptRequest(const QUrl &url, QWebEngineUrlRequestInfo::ResourceType resourceType, QWebEngineUrlRequestInfo::NavigationType navigationType);
    /*! Sets whether the interceptor is enabled. */
    void setEnabled(bool enabled);
Q_SIGNALS:
    /*! Emitted when a form submission is forbidden. */
    void formSubmittedForbidden();

private:
    bool mEnabled = true;
};
}
