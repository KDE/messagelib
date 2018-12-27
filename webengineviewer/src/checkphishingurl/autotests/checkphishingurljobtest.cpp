/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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

#include <QSignalSpy>
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
    QTest::newRow("value") << QUrl(QStringLiteral("http://www.kde.org")) << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],\"threatEntries\":[{\"url\":\"http://www.kde.org\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}")
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
    QTest::addColumn<QByteArray>("input");
    QTest::addColumn<QUrl>("checkedUrl");
    QTest::addColumn<uint>("verifyCacheAfterThisTime");
    QTest::addColumn<WebEngineViewer::CheckPhishingUrlUtil::UrlStatus>("urlStatus");
    uint val = 0;
    QTest::newRow("empty") << QByteArray() << QUrl() << val << WebEngineViewer::CheckPhishingUrlUtil::Unknown;
    QTest::newRow("empty1") << QByteArray() << QUrl(QStringLiteral("http://www.kde.org")) << val << WebEngineViewer::CheckPhishingUrlUtil::Unknown;

    QTest::newRow("urlOk") << QByteArrayLiteral("{}") << QUrl(QStringLiteral("http://www.kde.org")) << val << WebEngineViewer::CheckPhishingUrlUtil::Ok;

    val = WebEngineViewer::CheckPhishingUrlUtil::refreshingCacheAfterThisTime(WebEngineViewer::CheckPhishingUrlUtil::convertToSecond(QStringLiteral("300s")));
    QTest::newRow("malware") << QByteArrayLiteral(
        "{\"matches\":[{\"threatType\":\"MALWARE\",\"platformType\":\"WINDOWS\",\"threat\":{\"url\":\"http://malware.testing.google.test/testing/malware/\"},\"cacheDuration\":\"300s\",\"threatEntryType\":\"URL\"}]}")
                             << QUrl(QStringLiteral("http://malware.testing.google.test/testing/malware/")) << val << WebEngineViewer::CheckPhishingUrlUtil::MalWare;
}

void CheckPhishingUrlJobTest::shouldParseResult()
{
    QFETCH(QByteArray, input);
    QFETCH(QUrl, checkedUrl);
    QFETCH(uint, verifyCacheAfterThisTime);
    QFETCH(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus, urlStatus);

    WebEngineViewer::CheckPhishingUrlJob job;
    job.setUrl(checkedUrl);
    QSignalSpy spy1(&job, SIGNAL(result(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus,QUrl,uint)));
    job.parse(input);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy1.at(0).at(0).value<WebEngineViewer::CheckPhishingUrlUtil::UrlStatus>(), urlStatus);
    QCOMPARE(spy1.at(0).at(1).toUrl(), checkedUrl);
    QCOMPARE(spy1.at(0).at(2).value<uint>(), verifyCacheAfterThisTime);
}

QTEST_MAIN(CheckPhishingUrlJobTest)
