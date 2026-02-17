/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QMetaType>
#include <QString>
namespace WebEngineViewer
{
namespace CheckPhishingUrlUtil
{
enum UrlStatus : uint8_t {
    Ok = 0,
    MalWare = 1,
    BrokenNetwork = 2,
    InvalidUrl = 3,
    Unknown = 4,
};

[[nodiscard]] QString apiKey();
/*! Returns the version of the application. */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString versionApps();
[[nodiscard]] QString databaseFileName();
/*! Returns the configuration file name. */
[[nodiscard]] WEBENGINEVIEWER_EXPORT QString configFileName();
/*! Returns the minor version number. */
[[nodiscard]] WEBENGINEVIEWER_EXPORT quint16 minorVersion();
/*! Returns the major version number. */
[[nodiscard]] WEBENGINEVIEWER_EXPORT quint16 majorVersion();
/*! Converts the given string to seconds. */
[[nodiscard]] WEBENGINEVIEWER_EXPORT double convertToSecond(const QString &str);
/*! Returns the time in seconds to refresh the cache after. */
WEBENGINEVIEWER_EXPORT uint refreshingCacheAfterThisTime(double seconds);
/*! Returns whether the cached value is still valid. */
[[nodiscard]] WEBENGINEVIEWER_EXPORT bool cachedValueStillValid(uint seconds);
/*! Generates a random second value based on the number of failed attempts. */
[[nodiscard]] WEBENGINEVIEWER_EXPORT int generateRandomSecondValue(int numberOfFailed);
}
}
Q_DECLARE_METATYPE(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus)
