/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

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
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString findAllImages();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString findAllScripts();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString findAllAnchors();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString findAllAnchorsAndForms();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString searchElementPosition(const QString &elementStr);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString scrollToPosition(const QPoint &pos);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString setElementByIdVisible(const QString &elementStr, bool visibility);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString setStyleToElement(const QString &elementStr, const QString &style);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString scrollDown(int pixel);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString scrollUp(int pixel);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString scrollPercentage(int percent);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString scrollToRelativePosition(qreal pos);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString removeStyleToElement(const QString &element);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString isScrolledToBottom();
}
}
