/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

#include "cidreferencesurlinterceptor.h"
#include "htmlwriter/webengineembedpart.h"

#include <QWebEngineUrlRequestInfo>

using namespace MessageViewer;

CidReferencesUrlInterceptor::CidReferencesUrlInterceptor(QObject *parent)
    : WebEngineViewer::NetworkPluginUrlInterceptorInterface(parent)
{
}

CidReferencesUrlInterceptor::~CidReferencesUrlInterceptor()
{
}

bool CidReferencesUrlInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    const QUrl urlRequestUrl(info.requestUrl());
    if (urlRequestUrl.scheme() == QLatin1String("cid")) {
        const QString newUrl = MessageViewer::WebEngineEmbedPart::self()->contentUrl(
            urlRequestUrl.path());
        if (!newUrl.isEmpty()) {
            info.redirect(QUrl(newUrl));
        }
    }
    return false;
}
