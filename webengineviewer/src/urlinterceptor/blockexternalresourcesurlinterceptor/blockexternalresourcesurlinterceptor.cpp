/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blockexternalresourcesurlinterceptor.h"
using namespace WebEngineViewer;

BlockExternalResourcesUrlInterceptor::BlockExternalResourcesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
}

BlockExternalResourcesUrlInterceptor::~BlockExternalResourcesUrlInterceptor() = default;

void BlockExternalResourcesUrlInterceptor::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

bool BlockExternalResourcesUrlInterceptor::interceptRequest(const QUrl &url,
                                                            QWebEngineUrlRequestInfo::ResourceType resourceType,
                                                            QWebEngineUrlRequestInfo::NavigationType navigationType)
{
    if (!mEnabled) {
        return false;
    }
    const QString scheme = url.scheme();
    if (scheme == QLatin1StringView("data") || scheme == QLatin1StringView("file")) {
        return false;
    }

    if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeMedia || resourceType == QWebEngineUrlRequestInfo::ResourceTypePing
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypePrefetch || resourceType == QWebEngineUrlRequestInfo::ResourceTypeFavicon
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeXhr || resourceType == QWebEngineUrlRequestInfo::ResourceTypeObject
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeScript || resourceType == QWebEngineUrlRequestInfo::ResourceTypeServiceWorker
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeSharedWorker || resourceType == QWebEngineUrlRequestInfo::ResourceTypeWorker
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeSubResource || resourceType == QWebEngineUrlRequestInfo::ResourceTypePluginResource
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeCspReport || resourceType == QWebEngineUrlRequestInfo::ResourceTypeUnknown) {
        return true;
    } else if (navigationType == QWebEngineUrlRequestInfo::NavigationTypeFormSubmitted) {
        Q_EMIT formSubmittedForbidden();
        return true;
    } else if (navigationType == QWebEngineUrlRequestInfo::NavigationTypeReload || navigationType == QWebEngineUrlRequestInfo::NavigationTypeTyped
               || navigationType == QWebEngineUrlRequestInfo::NavigationTypeBackForward || navigationType == QWebEngineUrlRequestInfo::NavigationTypeOther
               || navigationType == QWebEngineUrlRequestInfo::NavigationTypeRedirect) {
        return true;
    }
    return false;
}

bool BlockExternalResourcesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QWebEngineUrlRequestInfo::ResourceType resourceType = info.resourceType();
    const QWebEngineUrlRequestInfo::NavigationType navigationType = info.navigationType();

    return interceptRequest(info.requestUrl(), resourceType, navigationType);
}

#include "moc_blockexternalresourcesurlinterceptor.cpp"
