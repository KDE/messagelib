/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "loadexternalreferencesurlinterceptor.h"
#include "remote-content/remotecontentmanager.h"
#include <QDebug>
#include <QWebEngineUrlRequestInfo>

using namespace MessageViewer;

LoadExternalReferencesUrlInterceptor::LoadExternalReferencesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
}

LoadExternalReferencesUrlInterceptor::~LoadExternalReferencesUrlInterceptor()
{
}

bool LoadExternalReferencesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QString scheme = info.requestUrl().scheme();
    if (scheme == QLatin1String("data") || scheme == QLatin1String("file")) {
        return false;
    }
    if (mAllowLoadExternalReference) {
        if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage && !info.requestUrl().isLocalFile() && (scheme != QLatin1String("cid"))) {
            const QUrl url = info.requestUrl().adjusted(QUrl::RemovePath | QUrl::RemovePort | QUrl::RemoveQuery);
            bool contains = false;
            if (MessageViewer::RemoteContentManager::self()->isBlocked(url, contains)) {
                return true;
            }
        }
        return false;
    } else {
        if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage && !info.requestUrl().isLocalFile() && (scheme != QLatin1String("cid"))) {
            const QUrl url = info.requestUrl().adjusted(QUrl::RemovePath | QUrl::RemovePort | QUrl::RemoveQuery);
            bool contains = false;
            if (MessageViewer::RemoteContentManager::self()->isAutorized(url, contains)) {
                return false;
            }
            if (!contains) {
                Q_EMIT urlBlocked(info.requestUrl());
            }
            return true;
        } else if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeFontResource) {
            return true;
        } else if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeStylesheet) {
            return true;
        }
    }
    return false;
}

void LoadExternalReferencesUrlInterceptor::setAllowExternalContent(bool b)
{
    mAllowLoadExternalReference = b;
}

bool LoadExternalReferencesUrlInterceptor::allowExternalContent() const
{
    return mAllowLoadExternalReference;
}
