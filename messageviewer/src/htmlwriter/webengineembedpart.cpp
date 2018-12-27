/*
  Copyright (c) 2016-2019 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
