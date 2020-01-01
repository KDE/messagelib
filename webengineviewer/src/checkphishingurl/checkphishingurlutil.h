/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

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

#ifndef CHECKPHISHINGURLUTIL_H
#define CHECKPHISHINGURLUTIL_H

#include <QString>
#include <QObject>
#include "webengineviewer_export.h"
namespace WebEngineViewer {
namespace CheckPhishingUrlUtil {
enum UrlStatus {
    Ok = 0,
    MalWare = 1,
    BrokenNetwork = 2,
    InvalidUrl = 3,
    Unknown = 4
};

QString apiKey();
QString versionApps();
QString databaseFileName();
WEBENGINEVIEWER_EXPORT QString configFileName();
WEBENGINEVIEWER_EXPORT quint16 minorVersion();
WEBENGINEVIEWER_EXPORT quint16 majorVersion();
WEBENGINEVIEWER_EXPORT double convertToSecond(const QString &str);
WEBENGINEVIEWER_EXPORT uint refreshingCacheAfterThisTime(double seconds);
WEBENGINEVIEWER_EXPORT bool cachedValueStillValid(uint seconds);
WEBENGINEVIEWER_EXPORT int generateRandomSecondValue(int numberOfFailed);
}
}
Q_DECLARE_METATYPE(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus)
#endif // CHECKPHISHINGURLUTIL_H
