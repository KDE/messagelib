/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkphishingurlutil.h"
#include <QDateTime>
#include <QtMath>

using namespace WebEngineViewer;
QString CheckPhishingUrlUtil::apiKey()
{
    return QStringLiteral("AIzaSyBS62pXATjabbH2RM_jO2EzDg1mTMHlnyo");
}

QString CheckPhishingUrlUtil::versionApps()
{
    return QStringLiteral(WEBENGINEVIEWER_VERSION_STRING);
}

QString CheckPhishingUrlUtil::databaseFileName()
{
    return QStringLiteral("malware.db");
}

quint16 CheckPhishingUrlUtil::majorVersion()
{
    return 1;
}

quint16 CheckPhishingUrlUtil::minorVersion()
{
    return 0;
}

QString CheckPhishingUrlUtil::configFileName()
{
    return QStringLiteral("phishingurlrc");
}

double CheckPhishingUrlUtil::convertToSecond(const QString &str)
{
    QString minimumDuration = str;

    if (!minimumDuration.isEmpty()) {
        if (minimumDuration.endsWith(QLatin1Char('s'))) {
            minimumDuration = minimumDuration.left(minimumDuration.length() - 1);
        }
        bool ok;
        double val = minimumDuration.toDouble(&ok);
        if (ok) {
            return val;
        }
    }
    return -1;
}

uint CheckPhishingUrlUtil::refreshingCacheAfterThisTime(double seconds)
{
    if (seconds > 0) {
        return QDateTime::currentDateTime().addMSecs(seconds * 1000).toSecsSinceEpoch();
    } else {
        return 0;
    }
}

bool CheckPhishingUrlUtil::cachedValueStillValid(uint seconds)
{
    return QDateTime::currentDateTimeUtc().toSecsSinceEpoch() < seconds;
}

int CheckPhishingUrlUtil::generateRandomSecondValue(int numberOfFailed)
{
    // Random between 0-1
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) + 1;

    const int numberOfSecondByDay = 24 * 60 * 60;
    // MIN(((2^(n-1))*15 minutes) * (RAND + 1), 24 hours)
    int seconds = 0;
    if (numberOfFailed >= 1 && numberOfFailed < 9) {
        seconds = static_cast<int>(qMin(static_cast<int>(qPow(2, numberOfFailed - 1)) * (15 * 60) * r, static_cast<float>(numberOfSecondByDay)));
    } else if (numberOfFailed >= 9) {
        seconds = numberOfSecondByDay;
    }
    return seconds;
}
