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
#ifndef WEBENGINESCRIPT_H
#define WEBENGINESCRIPT_H

#include <QString>
#include <QPoint>
#include "webengineviewer_export.h"

namespace WebEngineViewer {
namespace WebEngineScript {
WEBENGINEVIEWER_EXPORT QString findAllImages();
WEBENGINEVIEWER_EXPORT QString findAllScripts();
WEBENGINEVIEWER_EXPORT QString findAllAnchors();
WEBENGINEVIEWER_EXPORT QString findAllAnchorsAndForms();
WEBENGINEVIEWER_EXPORT QString searchElementPosition(const QString &elementStr);
WEBENGINEVIEWER_EXPORT QString scrollToPosition(const QPoint &pos);
WEBENGINEVIEWER_EXPORT QString setElementByIdVisible(const QString &elementStr, bool visibility);
WEBENGINEVIEWER_EXPORT QString setStyleToElement(const QString &elementStr, const QString &style);
WEBENGINEVIEWER_EXPORT QString scrollDown(int pixel);
WEBENGINEVIEWER_EXPORT QString scrollUp(int pixel);
WEBENGINEVIEWER_EXPORT QString scrollPercentage(int percent);
WEBENGINEVIEWER_EXPORT QString scrollToRelativePosition(qreal pos);
WEBENGINEVIEWER_EXPORT QString removeStyleToElement(const QString &element);
WEBENGINEVIEWER_EXPORT QString isScrolledToBottom();
}
}
#endif // WEBENGINESCRIPT_H
