/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

using namespace MessageViewer;

WebHitTestResult::WebHitTestResult(const QPoint &pos, const QUrl &url, const QVariant &result)
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

QString WebHitTestResult::alternateText() const
{
    return m_alternateText;
}

QRect WebHitTestResult::boundingRect() const
{
    return m_boundingRect;
}

QUrl WebHitTestResult::imageUrl() const
{
    return m_imageUrl;
}

bool WebHitTestResult::isContentEditable() const
{
    return m_isContentEditable;
}

bool WebHitTestResult::isContentSelected() const
{
    return m_isContentSelected;
}

bool WebHitTestResult::isNull() const
{
    return m_isNull;
}

QString WebHitTestResult::linkTitle() const
{
    return m_linkTitle;
}

QUrl WebHitTestResult::linkUrl() const
{
    return m_linkUrl;
}

QUrl WebHitTestResult::mediaUrl() const
{
    return m_mediaUrl;
}

bool WebHitTestResult::mediaPaused() const
{
    return m_mediaPaused;
}

bool WebHitTestResult::mediaMuted() const
{
    return m_mediaMuted;
}

QPoint WebHitTestResult::pos() const
{
    return m_pos;
}

QString WebHitTestResult::tagName() const
{
    return m_tagName;
}

void WebHitTestResult::init(const QVariantMap &map)
{
    if (map.isEmpty()) {
        return;
    }
    qDebug()<<" void WebHitTestResult::init(const QVariantMap &map)"<<map;
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
}

