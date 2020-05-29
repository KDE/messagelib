/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

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

#include <QBuffer>
#include <QUrl>
#include <QImage>
#include <QDebug>
#include <QWebEngineUrlRequestInfo>

using namespace MessageViewer;
#define LOAD_FROM_FILE 1
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
        if (info.resourceType() == QWebEngineUrlRequestInfo::ResourceTypeImage) {
            const QUrl newUrl = QUrl(MessageViewer::WebEngineEmbedPart::self()->contentUrl(
                        urlRequestUrl.path()));
            if (!newUrl.isEmpty()) {
#ifdef LOAD_FROM_FILE
                QImage image(newUrl.path());
                QByteArray ba;
                QBuffer buf(&ba);
                image.save(&buf, "png");
                const QByteArray hexed = ba.toBase64();
                buf.close();
                const QUrl r = QUrl(QString::fromUtf8(QByteArray("data:image/png;base64,") + hexed));
                info.redirect(r);
#else
                info.redirect(newUrl);
#endif
            }
        }
    }
    return false;
}
