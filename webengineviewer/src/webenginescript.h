/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "webengineviewer_export.h"
#include <QPoint>
#include <QString>

namespace WebEngineViewer
{
namespace WebEngineScript
{
/*!
 * Returns JavaScript code to find all images in the document.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString findAllImages();
/*!
 * Returns JavaScript code to find all scripts in the document.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString findAllScripts();
/*!
 * Returns JavaScript code to find all anchors in the document.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString findAllAnchors();
/*!
 * Returns JavaScript code to find all anchors and forms in the document.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString findAllAnchorsAndForms();
/*!
 * Returns JavaScript code to search for the position of an element.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString searchElementPosition(const QString &elementStr);
/*!
 * Returns JavaScript code to scroll to a specific position.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollToPosition(const QPoint &pos);
/*!
 * Returns JavaScript code to set the visibility of an element by its ID.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString setElementByIdVisible(const QString &elementStr, bool visibility);
/*!
 * Returns JavaScript code to set a style to an element.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString setStyleToElement(const QString &elementStr, const QString &style);
/*!
 * Returns JavaScript code to scroll down by the specified number of pixels.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollDown(int pixel);
/*!
 * Returns JavaScript code to scroll up by the specified number of pixels.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollUp(int pixel);
/*!
 * Returns JavaScript code to scroll by a percentage.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollPercentage(int percent);
/*!
 * Returns JavaScript code to scroll to a relative position.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollToRelativePosition(qreal pos);
/*!
 * Returns JavaScript code to remove a style from an element.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString removeStyleToElement(const QString &element);
/*!
 * Returns JavaScript code to check if the document is scrolled to the bottom.
 */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString isScrolledToBottom();
}
}
