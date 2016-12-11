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

#include "searchfullhashjobtest.h"
#include "../searchfullhashjob.h"
#include "../checkphishingurlutil.h"
#include <QTest>

SearchFullHashJobTest::SearchFullHashJobTest(QObject *parent)
    : QObject(parent)
{

}

SearchFullHashJobTest::~SearchFullHashJobTest()
{

}

void SearchFullHashJobTest::shouldNotBeAbleToStartWithEmptyUrl()
{
    WebEngineViewer::SearchFullHashJob job;
    QVERIFY(!job.canStart());
}

void SearchFullHashJobTest::shouldCreateRequest_data()
{
    QTest::addColumn<QHash<QByteArray, QByteArray> >("hash");
    QTest::addColumn<QStringList>("databaseHash");
    QTest::addColumn<QString>("request");
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<bool>("canStart");
    QTest::newRow("no hash") << QHash<QByteArray, QByteArray>() << QStringList() << QString() << QUrl() << false;
    QTest::newRow("database hash but not hash and not url") << QHash<QByteArray, QByteArray>() << QStringList{QStringLiteral("boo")} << QString() << QUrl() << false;
    QHash<QByteArray, QByteArray> hashs;
    hashs.insert(QByteArrayLiteral("bla"), QByteArrayLiteral("bla"));
    QTest::newRow("database hash but hash and not url") << hashs << QStringList{QStringLiteral("boo")} << QString() << QUrl() << false;
    QTest::newRow("database hash and hash") << hashs << QStringList{QStringLiteral("boo")}
                                            << QStringLiteral("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"clientStates\":[\"boo\"],\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],\"threatEntries\":[{\"hash\":\"bla\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}").arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps())
                                            << QUrl(QStringLiteral("http://www.kde.org")) << true;
    QTest::newRow("multi database hash and hash") << hashs << (QStringList() << QStringLiteral("boo") << QStringLiteral("bli"))
            << QStringLiteral("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"clientStates\":[\"boo\",\"bli\"],\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],\"threatEntries\":[{\"hash\":\"bla\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}").arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps())
            << QUrl(QStringLiteral("http://www.kde.org")) << true;
}

void SearchFullHashJobTest::shouldCreateRequest()
{
    typedef QHash<QByteArray, QByteArray> hashdef;
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

QTEST_MAIN(SearchFullHashJobTest)
