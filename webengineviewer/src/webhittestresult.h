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
#ifndef WebHitTestResult_H
#define WebHitTestResult_H

#include "webengineviewer_export.h"
#include <QUrl>
#include <QRect>
#include <QString>
#include <QVariant>
#include <QWebEngineContextMenuData>

namespace WebEngineViewer {
class WebHitTestResultPrivate;
class WEBENGINEVIEWER_EXPORT WebHitTestResult
{
public:
    WebHitTestResult();
    WebHitTestResult(const QPoint &pos, const QUrl &pageUrl, const QVariant &result);
    WebHitTestResult(const WebHitTestResult &other);
    WebHitTestResult(const QPoint &pos, const QUrl &pageUrl, const QWebEngineContextMenuData &data);
    ~WebHitTestResult();

    QString alternateText() const;
    QRect boundingRect() const;
    QUrl imageUrl() const;
    bool isContentEditable() const;
    bool isContentSelected() const;
    bool isNull() const;
    QString linkTitle() const;
    QUrl linkUrl() const;
    QUrl mediaUrl() const;
    bool mediaPaused() const;
    bool mediaMuted() const;
    QPoint pos() const;
    QString tagName() const;
    QUrl pageUrl() const;

    WebHitTestResult &operator=(const WebHitTestResult &webHit);
private:
    WebHitTestResultPrivate *d;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::WebHitTestResult)
#endif // WEBHITTESTRESULT_H
