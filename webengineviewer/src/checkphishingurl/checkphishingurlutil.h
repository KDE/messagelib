/*
   SPDX-FileCopyrightText: 2016-2022 Laurent Montel <montel@kde.org>

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
enum UrlStatus { Ok = 0, MalWare = 1, BrokenNetwork = 2, InvalidUrl = 3, Unknown = 4 };

Q_REQUIRED_RESULT QString apiKey();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString versionApps();
Q_REQUIRED_RESULT QString databaseFileName();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT QString configFileName();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT quint16 minorVersion();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT quint16 majorVersion();
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT double convertToSecond(const QString &str);
WEBENGINEVIEWER_EXPORT uint refreshingCacheAfterThisTime(double seconds);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT bool cachedValueStillValid(uint seconds);
Q_REQUIRED_RESULT WEBENGINEVIEWER_EXPORT int generateRandomSecondValue(int numberOfFailed);
}
}
Q_DECLARE_METATYPE(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus)
