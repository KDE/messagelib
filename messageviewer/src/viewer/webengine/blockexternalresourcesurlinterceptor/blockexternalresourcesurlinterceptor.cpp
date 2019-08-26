/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "blockexternalresourcesurlinterceptor.h"

#include <QWebEngineUrlRequestInfo>

using namespace MessageViewer;

BlockExternalResourcesUrlInterceptor::BlockExternalResourcesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
}

BlockExternalResourcesUrlInterceptor::~BlockExternalResourcesUrlInterceptor()
{
}

bool BlockExternalResourcesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QString scheme = info.requestUrl().scheme();
    if (scheme == QLatin1String("data")
        || scheme == QLatin1String("file")) {
        return false;
    }
    const QWebEngineUrlRequestInfo::ResourceType resourceType = info.resourceType();
    const QWebEngineUrlRequestInfo::NavigationType navigationType = info.navigationType();
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
               || navigationType == QWebEngineUrlRequestInfo::NavigationTypeOther) {
        return true;
    }
    return false;
}
