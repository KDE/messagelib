/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "webengineembedpart.h"

using namespace MessageViewer;

WebEngineEmbedPart::WebEngineEmbedPart(QObject *parent)
    : QObject(parent)
{
}

WebEngineEmbedPart::~WebEngineEmbedPart()
{
}

WebEngineEmbedPart *WebEngineEmbedPart::self()
{
    static WebEngineEmbedPart s_self;
    return &s_self;
}

QString WebEngineEmbedPart::contentUrl(const QString &contentId) const
{
    return mEmbeddedPartMap.value(contentId);
}

void WebEngineEmbedPart::addEmbedPart(const QByteArray &contentId, const QString &contentURL)
{
    mEmbeddedPartMap[QLatin1String(contentId)] = contentURL;
}

void WebEngineEmbedPart::clear()
{
    mEmbeddedPartMap.clear();
}

bool WebEngineEmbedPart::isEmpty() const
{
    return mEmbeddedPartMap.isEmpty();
}

QMap<QString, QString> WebEngineEmbedPart::embeddedPartMap() const
{
    return mEmbeddedPartMap;
}
