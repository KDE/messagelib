/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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
#include <QDebug>

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
    //qDebug()<<" bool BlockExternalResourcesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)" << (info.navigationType() == QWebEngineUrlRequestInfo::NavigationTypeFormSubmitted);
    const QWebEngineUrlRequestInfo::ResourceType type = info.resourceType();
    if (type == QWebEngineUrlRequestInfo::ResourceTypeMedia ||
            type == QWebEngineUrlRequestInfo::ResourceTypePing ||
            type == QWebEngineUrlRequestInfo::ResourceTypePrefetch ||
            type == QWebEngineUrlRequestInfo::ResourceTypeFavicon ||
            type == QWebEngineUrlRequestInfo::ResourceTypeXhr ||
        #if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
            type == QWebEngineUrlRequestInfo::ResourceTypePluginResource ||
            type == QWebEngineUrlRequestInfo::ResourceTypeCspReport ||
        #endif
            type == QWebEngineUrlRequestInfo::ResourceTypeUnknown
            ) {
        return true;
    } else if (info.navigationType() == QWebEngineUrlRequestInfo::NavigationTypeFormSubmitted) {
        Q_EMIT formSubmittedForbidden();
        return true;
    }
    return false;
}
