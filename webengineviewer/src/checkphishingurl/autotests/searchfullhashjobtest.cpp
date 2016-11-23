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
    QTest::addColumn<QByteArray>("hash");
    QTest::addColumn<QStringList>("databaseHash");
    QTest::addColumn<QString>("request");
    QTest::addColumn<bool>("canStart");
    QTest::newRow("no hash") << QByteArray() << QStringList() << QString() << false;
    QTest::newRow("database hash but not hash") << QByteArray() << QStringList{QStringLiteral("boo")} << QString() << false;
    QTest::newRow("database hash and hash") << QByteArrayLiteral("bla") << QStringList{QStringLiteral("boo")}
                                            << QStringLiteral("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"clientStates\":[\"boo\"],\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],\"threatEntries\":[{\"hash\":\"bla\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}").arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps()) << true;
    QTest::newRow("multi database hash and hash") << QByteArrayLiteral("bla") << (QStringList() << QStringLiteral("boo") << QStringLiteral("bli"))
            << QStringLiteral("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"clientStates\":[\"boo\",\"bli\"],\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],\"threatEntries\":[{\"hash\":\"bla\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}").arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps()) << true;
}

void SearchFullHashJobTest::shouldCreateRequest()
{
    QFETCH(QByteArray, hash);
    QFETCH(QStringList, databaseHash);
    QFETCH(QString, request);
    QFETCH(bool, canStart);
    WebEngineViewer::SearchFullHashJob job;
    job.setDatabaseState(databaseHash);
    job.setSearchHash(hash);
    QCOMPARE(job.canStart(), canStart);
    if (canStart) {
        QCOMPARE(job.jsonRequest(), request.toLatin1());
    }
}

QTEST_MAIN(SearchFullHashJobTest)
