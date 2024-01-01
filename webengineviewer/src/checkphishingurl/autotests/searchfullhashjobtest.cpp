/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "searchfullhashjobtest.h"
#include "../checkphishingurlutil.h"
#include "../searchfullhashjob.h"
#include <QTest>

SearchFullHashJobTest::SearchFullHashJobTest(QObject *parent)
    : QObject(parent)
{
}

SearchFullHashJobTest::~SearchFullHashJobTest() = default;

void SearchFullHashJobTest::shouldNotBeAbleToStartWithEmptyUrl()
{
    WebEngineViewer::SearchFullHashJob job;
    QVERIFY(!job.canStart());
}

void SearchFullHashJobTest::shouldCreateRequest_data()
{
    QTest::addColumn<QHash<QByteArray, QByteArray>>("hash");
    QTest::addColumn<QStringList>("databaseHash");
    QTest::addColumn<QString>("request");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<bool>("canStart");
    QTest::newRow("no hash") << QHash<QByteArray, QByteArray>() << QStringList() << QString() << QUrl() << false;
    QTest::newRow("database hash but not hash and not url")
        << QHash<QByteArray, QByteArray>() << QStringList{QStringLiteral("boo")} << QString() << QUrl() << false;
    QHash<QByteArray, QByteArray> hashes;
    hashes.insert(QByteArrayLiteral("bla"), QByteArrayLiteral("bla"));
    QTest::newRow("database hash but hash and not url") << hashes << QStringList{QStringLiteral("boo")} << QString() << QUrl() << false;
    QTest::newRow("database hash and hash")
        << hashes << QStringList{QStringLiteral("boo")}
        << QStringLiteral(
               "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"clientStates\":[\"boo\"],\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],"
               "\"threatEntries\":[{\"hash\":\"bla\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}")
               .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps())
        << QUrl(QStringLiteral("http://www.kde.org")) << true;
    QTest::newRow("multi database hash and hash")
        << hashes << (QStringList() << QStringLiteral("boo") << QStringLiteral("bli"))
        << QStringLiteral(
               "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"clientStates\":[\"boo\",\"bli\"],\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],"
               "\"threatEntries\":[{\"hash\":\"bla\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}")
               .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps())
        << QUrl(QStringLiteral("http://www.kde.org")) << true;
}

void SearchFullHashJobTest::shouldCreateRequest()
{
    using hashdef = QHash<QByteArray, QByteArray>;
    QFETCH(hashdef, hash);
    QFETCH(QStringList, databaseHash);
    QFETCH(QString, request);
    QFETCH(QUrl, url);
    QFETCH(bool, canStart);
    WebEngineViewer::SearchFullHashJob job;
    job.setDatabaseState(databaseHash);
    job.setSearchFullHashForUrl(url);
    if (!hash.isEmpty()) {
        job.setSearchHashs(hash);
    }
    QCOMPARE(job.canStart(), canStart);
    if (canStart) {
        QCOMPARE(job.jsonRequest(), request.toLatin1());
    }
}

QTEST_GUILESS_MAIN(SearchFullHashJobTest)

#include "moc_searchfullhashjobtest.cpp"
