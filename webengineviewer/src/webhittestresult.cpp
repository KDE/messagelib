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
    WebHitTestResultPrivate(const QWebEngineContextMenuData &data, const QPoint &pos = QPoint(), const QUrl &url = QUrl())
        : mIsNull(true)
        , mIsContentEditable(false)
        , mIsContentSelected(false)
        , mMediaPaused(false)
        , mMediaMuted(false)
        , mPos(pos)
        , mPageUrl(url)
    {
        init(data);
    }

    WebHitTestResultPrivate(const QPoint &pos = QPoint(), const QUrl &url = QUrl(), const QVariant &result = QVariant())
        : mIsNull(true)
        , mIsContentEditable(false)
        , mIsContentSelected(false)
        , mMediaPaused(false)
        , mMediaMuted(false)
        , mPos(pos)
        , mPageUrl(url)
    {
        init(result.toMap());
    }

    void init(const QVariantMap &map);
    void init(const QWebEngineContextMenuData &data);

    bool mIsNull;
    QString mAlternateText;
    QRect mBoundingRect;
    QUrl mImageUrl;
    bool mIsContentEditable;
    bool mIsContentSelected;
    QString mLinkTitle;
    QUrl mLinkUrl;
    QUrl mMediaUrl;
    bool mMediaPaused;
    bool mMediaMuted;
    QPoint mPos;
    QString mTagName;
    QUrl mPageUrl;
};

void WebHitTestResultPrivate::init(const QWebEngineContextMenuData &data)
{
    if (data.isValid()) {
        //TODO
    }
}

void WebHitTestResultPrivate::init(const QVariantMap &map)
{
    if (map.isEmpty()) {
        return;
    }
    //qDebug()<<" void WebHitTestResult::init(const QVariantMap &map)"<<map;
    mAlternateText = map.value(QStringLiteral("alternateText")).toString();
    mImageUrl = map.value(QStringLiteral("imageUrl")).toUrl();
    mIsContentEditable = map.value(QStringLiteral("contentEditable")).toBool();
    mIsContentSelected = map.value(QStringLiteral("contentSelected")).toBool();
    mLinkTitle = map.value(QStringLiteral("linkTitle")).toString();
    mLinkUrl = map.value(QStringLiteral("linkUrl")).toUrl();
    mMediaUrl = map.value(QStringLiteral("mediaUrl")).toUrl();
    mMediaPaused = map.value(QStringLiteral("mediaPaused")).toBool();
    mMediaMuted = map.value(QStringLiteral("mediaMuted")).toBool();
    mTagName = map.value(QStringLiteral("tagName")).toString();

    const QVariantList &rect = map.value(QStringLiteral("boundingRect")).toList();
    if (rect.size() == 4) {
        mBoundingRect = QRect(rect.at(0).toInt(), rect.at(1).toInt(), rect.at(2).toInt(), rect.at(3).toInt());
    }

    if (!mImageUrl.isEmpty()) {
        mImageUrl = mPageUrl.resolved(mImageUrl);
    }
    if (!mLinkUrl.isEmpty()) {
        mLinkUrl = mPageUrl.resolved(mLinkUrl);
    }
    if (!mMediaUrl.isEmpty()) {
        mMediaUrl = mPageUrl.resolved(mMediaUrl);
    }
    mIsNull = false;
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
    : d(new WebHitTestResultPrivate)
{
    (*this) = other;
}

WebHitTestResult::WebHitTestResult(const QPoint &pos, const QUrl &pageUrl, const QWebEngineContextMenuData &data)
    : d(new WebHitTestResultPrivate(data, pos, pageUrl))
{
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
    return d->mAlternateText;
}

QRect WebHitTestResult::boundingRect() const
{
    return d->mBoundingRect;
}

QUrl WebHitTestResult::imageUrl() const
{
    return d->mImageUrl;
}

bool WebHitTestResult::isContentEditable() const
{
    return d->mIsContentEditable;
}

bool WebHitTestResult::isContentSelected() const
{
    return d->mIsContentSelected;
}

bool WebHitTestResult::isNull() const
{
    return d->mIsNull;
}

QString WebHitTestResult::linkTitle() const
{
    return d->mLinkTitle;
}

QUrl WebHitTestResult::linkUrl() const
{
    return d->mLinkUrl;
}

QUrl WebHitTestResult::mediaUrl() const
{
    return d->mMediaUrl;
}

bool WebHitTestResult::mediaPaused() const
{
    return d->mMediaPaused;
}

bool WebHitTestResult::mediaMuted() const
{
    return d->mMediaMuted;
}

QPoint WebHitTestResult::pos() const
{
    return d->mPos;
}

QString WebHitTestResult::tagName() const
{
    return d->mTagName;
}

QUrl WebHitTestResult::pageUrl() const
{
    return d->mPageUrl;
}
