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

#include "checkphishingurljobtest.h"
#include "../checkphishingurljob.h"
#include "../checkphishingurlutil.h"

#include <QTest>

CheckPhishingUrlJobTest::CheckPhishingUrlJobTest(QObject *parent)
    : QObject(parent)
{

}

CheckPhishingUrlJobTest::~CheckPhishingUrlJobTest()
{

}

void CheckPhishingUrlJobTest::shouldNotBeAbleToStartWithEmptyUrl()
{
    WebEngineViewer::CheckPhishingUrlJob job;
    QVERIFY(!job.canStart());
}

void CheckPhishingUrlJobTest::shouldCreateRequest_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<QString>("request");
    QTest::addColumn<bool>("canStart");
    QTest::newRow("no url") << QUrl() << QString() << false;
    QTest::newRow("value") << QUrl(QStringLiteral("http://www.kde.org")) << QStringLiteral("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],\"threatEntries\":[{\"url\":\"http://www.kde.org\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}")
                           .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps()) << true;
}

void CheckPhishingUrlJobTest::shouldCreateRequest()
{
    QFETCH(QUrl, url);
    QFETCH(QString, request);
    QFETCH(bool, canStart);
    WebEngineViewer::CheckPhishingUrlJob job;
    job.setUrl(url);
    QCOMPARE(job.canStart(), canStart);
    if (canStart) {
        QCOMPARE(job.jsonRequest(), request.toLatin1());
    }
}

void CheckPhishingUrlJobTest::shouldParseResult_data()
{

}

void CheckPhishingUrlJobTest::shouldParseResult()
{

}

QTEST_MAIN(CheckPhishingUrlJobTest)
