/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "checkphishingurlutil.h"
#include "webengineviewer_version.h"
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

double CheckPhishingUrlUtil::convertToSecond(QString str)
{
    QString minimumDuration = str;

    if (!minimumDuration.isEmpty()) {
        if (minimumDuration.endsWith(QLatin1Char('s'))) {
            minimumDuration = minimumDuration.left(minimumDuration.length()-1);
        }
        bool ok;
        double val = minimumDuration.toDouble(&ok);
        if (ok) {
            return val;
        }
    }
    return -1;
}

