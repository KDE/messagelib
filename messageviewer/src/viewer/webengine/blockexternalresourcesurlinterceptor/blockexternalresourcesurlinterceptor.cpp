/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "blockexternalresourcesurlinterceptor.h"

#include <qtwebenginewidgetsversion.h>

using namespace MessageViewer;

BlockExternalResourcesUrlInterceptor::BlockExternalResourcesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
}

BlockExternalResourcesUrlInterceptor::~BlockExternalResourcesUrlInterceptor()
{
}

bool BlockExternalResourcesUrlInterceptor::interceptRequest(const QString &scheme, QWebEngineUrlRequestInfo::ResourceType resourceType, QWebEngineUrlRequestInfo::NavigationType navigationType)
{
    if (scheme == QLatin1String("data")
        || scheme == QLatin1String("file")) {
        return false;
    }

    if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeMedia
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypePing
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypePrefetch
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeFavicon
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeXhr
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeObject
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeScript
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeServiceWorker
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeSharedWorker
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeWorker
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeSubResource
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypePluginResource
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeCspReport
        || resourceType == QWebEngineUrlRequestInfo::ResourceTypeUnknown) {
        return true;
    } else if (navigationType == QWebEngineUrlRequestInfo::NavigationTypeFormSubmitted) {
        Q_EMIT formSubmittedForbidden();
        return true;
    } else if (navigationType == QWebEngineUrlRequestInfo::NavigationTypeReload
               || navigationType == QWebEngineUrlRequestInfo::NavigationTypeTyped
               || navigationType == QWebEngineUrlRequestInfo::NavigationTypeBackForward
               || navigationType == QWebEngineUrlRequestInfo::NavigationTypeOther
           #if QTWEBENGINEWIDGETS_VERSION >= QT_VERSION_CHECK(5, 14, 0)
               || navigationType == QWebEngineUrlRequestInfo::NavigationTypeRedirect
           #endif
               ) {
        return true;
    }
    return false;
}

bool BlockExternalResourcesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QString scheme = info.requestUrl().scheme();
    const QWebEngineUrlRequestInfo::ResourceType resourceType = info.resourceType();
    const QWebEngineUrlRequestInfo::NavigationType navigationType = info.navigationType();

    return interceptRequest(scheme, resourceType, navigationType);
}
