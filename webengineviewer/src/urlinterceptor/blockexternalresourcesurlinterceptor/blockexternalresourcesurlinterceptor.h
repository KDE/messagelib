/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

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
    explicit BlockExternalResourcesUrlInterceptor(QObject *parent = nullptr);
    ~BlockExternalResourcesUrlInterceptor() override;

    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    // Used by autotest only
    Q_REQUIRED_RESULT bool
    interceptRequest(const QUrl &url, QWebEngineUrlRequestInfo::ResourceType resourceType, QWebEngineUrlRequestInfo::NavigationType navigationType);
    void setEnabled(bool enabled);
Q_SIGNALS:
    void formSubmittedForbidden();

private:
    bool mEnabled = true;
};
}
