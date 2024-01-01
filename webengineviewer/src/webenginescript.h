/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString findAllImages();
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString findAllScripts();
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString findAllAnchors();
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString findAllAnchorsAndForms();
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString searchElementPosition(const QString &elementStr);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollToPosition(const QPoint &pos);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString setElementByIdVisible(const QString &elementStr, bool visibility);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString setStyleToElement(const QString &elementStr, const QString &style);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollDown(int pixel);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollUp(int pixel);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollPercentage(int percent);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString scrollToRelativePosition(qreal pos);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString removeStyleToElement(const QString &element);
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString isScrolledToBottom();
}
}
