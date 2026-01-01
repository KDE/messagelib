/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "cidreferencesurlinterceptor.h"
#include "htmlwriter/webengineembedpart.h"

#include <QUrl>
#include <QWebEngineUrlRequestInfo>

// This class is not needed if we get cidschemehandler working,
// which could also help us avoid having to giving local file access.

using namespace MessageViewer;
CidReferencesUrlInterceptor::CidReferencesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
}

CidReferencesUrlInterceptor::~CidReferencesUrlInterceptor() = default;

bool CidReferencesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QUrl urlRequestUrl(info.requestUrl());
    if (urlRequestUrl.scheme() == QLatin1StringView("cid")) {
        if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage) {
            const QUrl newUrl = QUrl(MessageViewer::WebEngineEmbedPart::self()->contentUrl(urlRequestUrl.path()));
            if (!newUrl.isEmpty()) {
                info.redirect(newUrl);
            }
        }
    }
    return false;
}

#include "moc_cidreferencesurlinterceptor.cpp"
