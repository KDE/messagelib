/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "loadexternalreferencesurlinterceptor.h"
#include "remote-content/remotecontentmanager.h"

using namespace MessageViewer;

LoadExternalReferencesUrlInterceptor::LoadExternalReferencesUrlInterceptor(QObject *parent)
    : WebEngineViewer::LoadExternalReferencesUrlInterceptor(parent)
{
}

LoadExternalReferencesUrlInterceptor::~LoadExternalReferencesUrlInterceptor() = default;
bool LoadExternalReferencesUrlInterceptor::urlIsBlocked(const QUrl &requestedUrl)
{
    const QUrl url = requestedUrl.adjusted(QUrl::RemovePath | QUrl::RemovePort | QUrl::RemoveQuery);
    bool contains = false;
    if (MessageViewer::RemoteContentManager::self()->isBlocked(url, contains)) {
        return true;
    }
    return false;
}

bool LoadExternalReferencesUrlInterceptor::urlIsAuthorized(const QUrl &requestedUrl)
{
    const QUrl url = requestedUrl.adjusted(QUrl::RemovePath | QUrl::RemovePort | QUrl::RemoveQuery);
    bool contains = false;
    if (MessageViewer::RemoteContentManager::self()->isAutorized(url, contains)) {
        return false;
    }
    if (!contains) {
        Q_EMIT urlBlocked(requestedUrl);
    }
    return true;
}

#include "moc_loadexternalreferencesurlinterceptor.cpp"
