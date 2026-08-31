/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "loadexternalreferencesurlinterceptor.h"
#include <QWebEngineUrlRequestInfo>

using namespace Qt::Literals::StringLiterals;

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
    if (scheme == "data"_L1 || scheme == "file"_L1 || scheme == "qrc"_L1) {
        return false;
    }
    const QWebEngineUrlRequestInfo::ResourceType resourceType{info.resourceType()};
    if (mAllowLoadExternalReference) {
        if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeImage && !requestUrl.isLocalFile() && (scheme != "cid"_L1)) {
            return urlIsBlocked(requestUrl);
        }
        return false;
    } else {
        if (resourceType == QWebEngineUrlRequestInfo::ResourceTypeImage && !requestUrl.isLocalFile() && (scheme != "cid"_L1)) {
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
