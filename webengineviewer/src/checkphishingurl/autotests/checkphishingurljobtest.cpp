/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkphishingurljobtest.h"
using namespace Qt::Literals::StringLiterals;

#include "../checkphishingurljob.h"
#include "../checkphishingurlutil.h"

#include <QSignalSpy>
#include <QTest>

CheckPhishingUrlJobTest::CheckPhishingUrlJobTest(QObject *parent)
    : QObject(parent)
{
}

CheckPhishingUrlJobTest::~CheckPhishingUrlJobTest() = default;

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
    QTest::newRow("value") << QUrl(u"http://www.kde.org"_s)
                           << QStringLiteral(
                                  "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],"
                                  "\"threatEntries\":[{\"url\":\"http://www.kde.org\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}")
                                  .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps())
                           << true;
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
    QTest::newRow("empty1") << QByteArray() << QUrl(u"http://www.kde.org"_s) << val << WebEngineViewer::CheckPhishingUrlUtil::Unknown;

    QTest::newRow("urlOk") << QByteArrayLiteral("{}") << QUrl(u"http://www.kde.org"_s) << val << WebEngineViewer::CheckPhishingUrlUtil::Ok;

    val = WebEngineViewer::CheckPhishingUrlUtil::refreshingCacheAfterThisTime(WebEngineViewer::CheckPhishingUrlUtil::convertToSecond(u"300s"_s));
    QTest::newRow("malware") << QByteArrayLiteral(
        "{\"matches\":[{\"threatType\":\"MALWARE\",\"platformType\":\"WINDOWS\",\"threat\":{\"url\":\"http://malware.testing.google.test/testing/malware/"
        "\"},\"cacheDuration\":\"300s\",\"threatEntryType\":\"URL\"}]}")
                             << QUrl(u"http://malware.testing.google.test/testing/malware/"_s) << val << WebEngineViewer::CheckPhishingUrlUtil::MalWare;
}

void CheckPhishingUrlJobTest::shouldParseResult()
{
    QFETCH(QByteArray, input);
    QFETCH(QUrl, checkedUrl);
    QFETCH(uint, verifyCacheAfterThisTime);
    QFETCH(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus, urlStatus);

    WebEngineViewer::CheckPhishingUrlJob job;
    job.setUrl(checkedUrl);
    QSignalSpy spy1(&job, &WebEngineViewer::CheckPhishingUrlJob::result);
    job.parse(input);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy1.at(0).at(0).value<WebEngineViewer::CheckPhishingUrlUtil::UrlStatus>(), urlStatus);
    QCOMPARE(spy1.at(0).at(1).toUrl(), checkedUrl);
    QCOMPARE(spy1.at(0).at(2).value<uint>(), verifyCacheAfterThisTime);
}

QTEST_MAIN(CheckPhishingUrlJobTest)

#include "moc_checkphishingurljobtest.cpp"
