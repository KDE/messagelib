/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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
/**
 * @brief The WebHitTestResult class
 */
class WEBENGINEVIEWER_EXPORT WebHitTestResult
{
public:
    WebHitTestResult();
    WebHitTestResult(const QPoint &pos, const QUrl &pageUrl, const QVariant &result);
    WebHitTestResult(const WebHitTestResult &other);
    ~WebHitTestResult();

    [[nodiscard]] QString alternateText() const;
    [[nodiscard]] QRect boundingRect() const;
    [[nodiscard]] QUrl imageUrl() const;
    [[nodiscard]] bool isContentEditable() const;
    [[nodiscard]] bool isContentSelected() const;
    [[nodiscard]] bool isNull() const;
    [[nodiscard]] QString linkTitle() const;
    [[nodiscard]] QUrl linkUrl() const;
    [[nodiscard]] QUrl mediaUrl() const;
    [[nodiscard]] bool mediaPaused() const;
    [[nodiscard]] bool mediaMuted() const;
    [[nodiscard]] QPoint pos() const;
    [[nodiscard]] QString tagName() const;
    [[nodiscard]] QUrl pageUrl() const;

    WebHitTestResult &operator=(const WebHitTestResult &webHit);

private:
    std::unique_ptr<WebHitTestResultPrivate> const d;
};
}
Q_DECLARE_METATYPE(WebEngineViewer::WebHitTestResult)
