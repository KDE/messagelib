/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

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
WEBENGINEVIEWER_EXPORT Q_REQUIRED_RESULT QString versionApps();
Q_REQUIRED_RESULT QString databaseFileName();
WEBENGINEVIEWER_EXPORT Q_REQUIRED_RESULT QString configFileName();
WEBENGINEVIEWER_EXPORT Q_REQUIRED_RESULT quint16 minorVersion();
WEBENGINEVIEWER_EXPORT Q_REQUIRED_RESULT quint16 majorVersion();
WEBENGINEVIEWER_EXPORT Q_REQUIRED_RESULT double convertToSecond(const QString &str);
WEBENGINEVIEWER_EXPORT uint refreshingCacheAfterThisTime(double seconds);
WEBENGINEVIEWER_EXPORT Q_REQUIRED_RESULT bool cachedValueStillValid(uint seconds);
WEBENGINEVIEWER_EXPORT Q_REQUIRED_RESULT int generateRandomSecondValue(int numberOfFailed);
}
}
Q_DECLARE_METATYPE(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus)
