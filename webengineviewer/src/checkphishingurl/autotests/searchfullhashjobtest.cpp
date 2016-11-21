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
    QTest::addColumn<QByteArray>("request");
    QTest::addColumn<bool>("canStart");
    QTest::newRow("no hash") << QByteArray() << QStringList() << QByteArray() << false;
    QTest::newRow("database hash but not hash") << QByteArray() << QStringList{QStringLiteral("boo")} << QByteArray() << false;
    QTest::newRow("database hash and hash") << QByteArrayLiteral("bla") << QStringList{QStringLiteral("boo")}
                                            << QByteArrayLiteral("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"clientStates\":[\"boo\"],\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],\"threatEntries\":[{\"hash\":\"bla\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}") << true;
    QTest::newRow("multi database hash and hash") << QByteArrayLiteral("bla") << (QStringList() << QStringLiteral("boo") << QStringLiteral("bli"))
                                            << QByteArrayLiteral("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"clientStates\":[\"boo\",\"bli\"],\"threatInfo\":{\"platformTypes\":[\"WINDOWS\"],\"threatEntries\":[{\"hash\":\"bla\"}],\"threatEntryTypes\":[\"URL\"],\"threatTypes\":[\"MALWARE\"]}}") << true;
}

void SearchFullHashJobTest::shouldCreateRequest()
{
    QFETCH(QByteArray, hash);
    QFETCH(QStringList, databaseHash);
    QFETCH(QByteArray, request);
    QFETCH(bool, canStart);
    WebEngineViewer::SearchFullHashJob job;
    job.setDatabaseState(databaseHash);
    job.setSearchHash(hash);
    QCOMPARE(job.canStart(), canStart);
    if (canStart) {
        QCOMPARE(job.jsonRequest(), request);
    }
}


QTEST_MAIN(SearchFullHashJobTest)
