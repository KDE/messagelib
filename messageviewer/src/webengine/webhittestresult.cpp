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
#include "webenginepage.h"
#include <QWebEnginePage>

using namespace MessageViewer;

WebHitTestResult::WebHitTestResult(WebEnginePage *page, const QPoint &pos)
    : m_isNull(true)
    , m_isContentEditable(false)
    , m_isContentSelected(false)
    , m_mediaPaused(false)
    , m_mediaMuted(false)
    , m_pos(pos)
{
    QString source = QStringLiteral("(function() {"
                                    "var e = document.elementFromPoint(%1, %2);"
                                    "if (!e)"
                                    "    return;"
                                    "function isMediaElement(e) {"
                                    "    return e.tagName == 'AUDIO' || e.tagName == 'VIDEO';"
                                    "}"
                                    "function isEditableElement(e) {"
                                    "    if (e.isContentEditable)"
                                    "        return true;"
                                    "    if (e.tagName == 'INPUT' || e.tagName == 'TEXTAREA')"
                                    "        return e.getAttribute('readonly') != 'readonly';"
                                    "    return false;"
                                    "}"
                                    "function isSelected(e) {"
                                    "    var selection = window.getSelection();"
                                    "    if (selection.type != 'Range')"
                                    "        return false;"
                                    "    return window.getSelection().containsNode(e, true);"
                                    "}"
                                    "var res = {"
                                    "    alternateText: e.getAttribute('alt'),"
                                    "    boundingRect: '',"
                                    "    imageUrl: '',"
                                    "    contentEditable: isEditableElement(e),"
                                    "    contentSelected: isSelected(e),"
                                    "    linkTitle: '',"
                                    "    linkUrl: '',"
                                    "    mediaUrl: '',"
                                    "    tagName: e.tagName.toLowerCase()"
                                    "};"
                                    "var r = e.getBoundingClientRect();"
                                    "res.boundingRect = [r.top, r.left, r.width, r.height];"
                                    "if (e.tagName == 'IMG')"
                                    "    res.imageUrl = e.getAttribute('src');"
                                    "if (e.tagName == 'A') {"
                                    "    res.linkTitle = e.text;"
                                    "    res.linkUrl = e.getAttribute('href');"
                                    "}"
                                    "while (e) {"
                                    "    if (res.linkTitle == '' && e.tagName == 'A')"
                                    "        res.linkTitle = e.text;"
                                    "    if (res.linkUrl == '' && e.tagName == 'A')"
                                    "        res.linkUrl = e.getAttribute('href');"
                                    "    if (res.mediaUrl == '' && isMediaElement(e)) {"
                                    "        res.mediaUrl = e.currentSrc;"
                                    "        res.mediaPaused = e.paused;"
                                    "        res.mediaMuted = e.muted;"
                                    "    }"
                                    "    e = e.parentElement;"
                                    "}"
                                    "return res;"
                                    "})()");

    const QString &js = source.arg(pos.x()).arg(pos.y());
    init(page->url(), page->execJavaScript(js).toMap());
}

WebHitTestResult::~WebHitTestResult()
{
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

void WebHitTestResult::init(const QUrl &url, const QVariantMap &map)
{
    if (map.isEmpty()) {
        return;
    }

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
        m_imageUrl = url.resolved(m_imageUrl);
    }
    if (!m_linkUrl.isEmpty()) {
        m_linkUrl = url.resolved(m_linkUrl);
    }
    if (!m_mediaUrl.isEmpty()) {
        m_mediaUrl = url.resolved(m_mediaUrl);
    }
}

