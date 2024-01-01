/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
#include <QString>
namespace WebEngineViewer
{
namespace CheckPhishingUrlUtil
{
enum UrlStatus {
    Ok = 0,
    MalWare = 1,
    BrokenNetwork = 2,
    InvalidUrl = 3,
    Unknown = 4,
};

[[nodiscard]] QString apiKey();
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString versionApps();
[[nodiscard]] QString databaseFileName();
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString configFileName();
[[nodiscard]] WEBENGINEVIEWER_EXPORT quint16 minorVersion();
[[nodiscard]] WEBENGINEVIEWER_EXPORT quint16 majorVersion();
[[nodiscard]] WEBENGINEVIEWER_EXPORT double convertToSecond(const QString &str);
WEBENGINEVIEWER_EXPORT uint refreshingCacheAfterThisTime(double seconds);
[[nodiscard]] WEBENGINEVIEWER_EXPORT bool cachedValueStillValid(uint seconds);
[[nodiscard]] WEBENGINEVIEWER_EXPORT int generateRandomSecondValue(int numberOfFailed);
}
}
Q_DECLARE_METATYPE(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus)
