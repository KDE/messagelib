/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

/* ============================================================
* QupZilla - QtWebEngine based browser
* Copyright (C) 2015 David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "webhittestresult.h"
#include <QDebug>

using namespace WebEngineViewer;

class WebEngineViewer::WebHitTestResultPrivate
{
public:
    WebHitTestResultPrivate(const QPoint &pos = QPoint(), const QUrl &url = QUrl(), const QVariant &result = QVariant())
        : m_isNull(true),
          m_isContentEditable(false),
          m_isContentSelected(false),
          m_mediaPaused(false),
          m_mediaMuted(false),
          m_pos(pos),
          m_pageUrl(url)
    {
        init(result.toMap());
    }
    void init(const QVariantMap &map);

    bool m_isNull;
    QString m_alternateText;
    QRect m_boundingRect;
    QUrl m_imageUrl;
    bool m_isContentEditable;
    bool m_isContentSelected;
    QString m_linkTitle;
    QUrl m_linkUrl;
    QUrl m_mediaUrl;
    bool m_mediaPaused;
    bool m_mediaMuted;
    QPoint m_pos;
    QString m_tagName;
    QUrl m_pageUrl;
};

void WebHitTestResultPrivate::init(const QVariantMap &map)
{
    if (map.isEmpty()) {
        return;
    }
    //qDebug()<<" void WebHitTestResult::init(const QVariantMap &map)"<<map;
    m_alternateText = map.value(QStringLiteral("alternateText")).toString();
    m_imageUrl = map.value(QStringLiteral("imageUrl")).toUrl();
    m_isContentEditable = map.value(QStringLiteral("contentEditable")).toBool();
    m_isContentSelected = map.value(QStringLiteral("contentSelected")).toBool();
    m_linkTitle = map.value(QStringLiteral("linkTitle")).toString();
    m_linkUrl = map.value(QStringLiteral("linkUrl")).toUrl();
    m_mediaUrl = map.value(QStringLiteral("mediaUrl")).toUrl();
    m_mediaPaused = map.value(QStringLiteral("mediaPaused")).toBool();
    m_mediaMuted = map.value(QStringLiteral("mediaMuted")).toBool();
    m_tagName = map.value(QStringLiteral("tagName")).toString();

    const QVariantList &rect = map.value(QStringLiteral("boundingRect")).toList();
    if (rect.size() == 4) {
        m_boundingRect = QRect(rect.at(0).toInt(), rect.at(1).toInt(), rect.at(2).toInt(), rect.at(3).toInt());
    }

    if (!m_imageUrl.isEmpty()) {
        m_imageUrl = m_pageUrl.resolved(m_imageUrl);
    }
    if (!m_linkUrl.isEmpty()) {
        m_linkUrl = m_pageUrl.resolved(m_linkUrl);
    }
    if (!m_mediaUrl.isEmpty()) {
        m_mediaUrl = m_pageUrl.resolved(m_mediaUrl);
    }
    m_isNull = false;
}
WebHitTestResult::WebHitTestResult()
    : d(new WebHitTestResultPrivate)
{

}

WebHitTestResult::WebHitTestResult(const QPoint &pos, const QUrl &pageUrl, const QVariant &result)
    : d(new WebHitTestResultPrivate(pos, pageUrl, result))
{
}

WebHitTestResult::WebHitTestResult(const WebHitTestResult &other)
    : d(other.d)
{
    d = new WebHitTestResultPrivate;
    (*this) = other;
}

WebHitTestResult::~WebHitTestResult()
{
    delete d;
}

WebHitTestResult &WebHitTestResult::operator=(const WebHitTestResult &other)
{
    if (this != &other) {
        *d = *(other.d);
    }
    return *this;
}

QString WebHitTestResult::alternateText() const
{
    return d->m_alternateText;
}

QRect WebHitTestResult::boundingRect() const
{
    return d->m_boundingRect;
}

QUrl WebHitTestResult::imageUrl() const
{
    return d->m_imageUrl;
}

bool WebHitTestResult::isContentEditable() const
{
    return d->m_isContentEditable;
}

bool WebHitTestResult::isContentSelected() const
{
    return d->m_isContentSelected;
}

bool WebHitTestResult::isNull() const
{
    return d->m_isNull;
}

QString WebHitTestResult::linkTitle() const
{
    return d->m_linkTitle;
}

QUrl WebHitTestResult::linkUrl() const
{
    return d->m_linkUrl;
}

QUrl WebHitTestResult::mediaUrl() const
{
    return d->m_mediaUrl;
}

bool WebHitTestResult::mediaPaused() const
{
    return d->m_mediaPaused;
}

bool WebHitTestResult::mediaMuted() const
{
    return d->m_mediaMuted;
}

QPoint WebHitTestResult::pos() const
{
    return d->m_pos;
}

QString WebHitTestResult::tagName() const
{
    return d->m_tagName;
}

QUrl WebHitTestResult::pageUrl() const
{
    return d->m_pageUrl;
}
