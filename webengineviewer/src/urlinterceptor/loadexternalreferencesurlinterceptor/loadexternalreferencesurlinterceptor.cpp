/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "loadexternalreferencesurlinterceptor.h"
#include <QWebEngineUrlRequestInfo>

using namespace WebEngineViewer;

LoadExternalReferencesUrlInterceptor::LoadExternalReferencesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
}

LoadExternalReferencesUrlInterceptor::~LoadExternalReferencesUrlInterceptor() = default;

bool LoadExternalReferencesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QUrl requestUrl = info.requestUrl();
    const QString scheme = requestUrl.scheme();
    if (scheme == QLatin1StringView("data") || scheme == QLatin1StringView("file") || scheme == QLatin1StringView("qrc")) {
        return false;
    }
    const QWebEngineUrlRequestInfo::ResourceType resourceType{info.resourceType()};
    if (mAllowLoadExternalReference) {
        if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeImage && !requestUrl.isLocalFile() && (scheme != QLatin1StringView("cid"))) {
            return urlIsBlocked(requestUrl);
        }
        return false;
    } else {
        if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeImage && !requestUrl.isLocalFile() && (scheme != QLatin1StringView("cid"))) {
            return urlIsAuthorized(requestUrl);
        } else if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeFontResource) {
            return true;
        } else if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeStylesheet) {
            return true;
        }
    }
    return false;
}

bool LoadExternalReferencesUrlInterceptor::urlIsBlocked(const QUrl &requestedUrl)
{
    Q_UNUSED(requestedUrl)
    return false;
}

bool LoadExternalReferencesUrlInterceptor::urlIsAuthorized(const QUrl &requestedUrl)
{
    Q_UNUSED(requestedUrl)
    return true;
}

void LoadExternalReferencesUrlInterceptor::setAllowExternalContent(bool b)
{
    mAllowLoadExternalReference = b;
}

bool LoadExternalReferencesUrlInterceptor::allowExternalContent() const
{
    return mAllowLoadExternalReference;
}

#include "moc_loadexternalreferencesurlinterceptor.cpp"
