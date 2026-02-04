/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

/* ============================================================
 * QupZilla - QtWebEngine based browser
 * SPDX-FileCopyrightText: 2015 David Rosca <nowrep@gmail.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ============================================================ */
#pragma once

#include "webengineviewer_export.h"
#include <QRect>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <memory>

namespace WebEngineViewer
{
class WebHitTestResultPrivate;
/*!
 * \class WebEngineViewer::WebHitTestResult
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/WebHitTestResult
 *
 * \brief The WebHitTestResult class
 */
class WEBENGINEVIEWER_EXPORT WebHitTestResult
{
public:
    /*! Constructs a default WebHitTestResult object. */
    WebHitTestResult();
    /*! Constructs a WebHitTestResult from the given position, page URL, and result variant. */
    WebHitTestResult(const QPoint &pos, const QUrl &pageUrl, const QVariant &result);
    /*! Constructs a WebHitTestResult by copying the other object. */
    WebHitTestResult(const WebHitTestResult &other);
    /*! Destroys the WebHitTestResult object. */
    ~WebHitTestResult();

    /*! Returns the alternate text. */
    [[nodiscard]] QString alternateText() const;
    /*! Returns the bounding rectangle. */
    [[nodiscard]] QRect boundingRect() const;
    /*! Returns the image URL. */
    [[nodiscard]] QUrl imageUrl() const;
    /*! Returns whether the content is editable. */
    [[nodiscard]] bool isContentEditable() const;
    /*! Returns whether content is selected. */
    [[nodiscard]] bool isContentSelected() const;
    /*! Returns whether this is a null result. */
    [[nodiscard]] bool isNull() const;
    /*! Returns the link title. */
    [[nodiscard]] QString linkTitle() const;
    /*! Returns the link URL. */
    [[nodiscard]] QUrl linkUrl() const;
    /*! Returns the media URL. */
    [[nodiscard]] QUrl mediaUrl() const;
    /*! Returns whether the media is paused. */
    [[nodiscard]] bool mediaPaused() const;
    /*! Returns whether the media is muted. */
    [[nodiscard]] bool mediaMuted() const;
    /*! Returns the position. */
    [[nodiscard]] QPoint pos() const;
    /*! Returns the tag name. */
    [[nodiscard]] QString tagName() const;
    /*! Returns the page URL. */
    [[nodiscard]] QUrl pageUrl() const;

    /*! Assigns the other result to this object. */
    WebHitTestResult &operator=(const WebHitTestResult &webHit);

private:
    std::unique_ptr<WebHitTestResultPrivate> const d;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::WebHitTestResult)
