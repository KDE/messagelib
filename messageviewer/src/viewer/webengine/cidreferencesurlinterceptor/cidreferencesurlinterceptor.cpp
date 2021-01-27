/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "cidreferencesurlinterceptor.h"
#include "htmlwriter/webengineembedpart.h"

#include <QBuffer>
#include <QImage>
#include <QUrl>
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
            const QUrl newUrl = QUrl(MessageViewer::WebEngineEmbedPart::self()->contentUrl(urlRequestUrl.path()));
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
