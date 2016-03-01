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
#ifndef WebHitTestResult_H
#define WebHitTestResult_H

#include "messageviewer_export.h"
#include <QObject>
#include <QUrl>
#include <QRect>
#include <QString>
#include <QVariantMap>
class QWebEnginePage;

namespace MessageViewer
{
class MESSAGEVIEWER_EXPORT WebHitTestResult
{
public:
    WebHitTestResult(const QPoint &pos, const QUrl &url, const QVariant &result);

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

private:
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
}
#endif // WEBHITTESTRESULT_H
