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

#include "createphishingurldatabasejobtest.h"
#include "../createphishingurldatabasejob.h"

#include <QTest>

QByteArray readJsonFile(const QString &jsonFile)
{
    QFile file(QLatin1String(CHECKPHISHINGURL_DATA_DIR) + QLatin1Char('/') + jsonFile);
    file.open(QIODevice::ReadOnly);
    Q_ASSERT(file.isOpen());
    const QByteArray data = file.readAll();
    Q_ASSERT(!data.isEmpty());
    return data;
}


CreatePhishingUrlDataBaseJobTest::CreatePhishingUrlDataBaseJobTest(QObject *parent)
    : QObject(parent)
{

}

CreatePhishingUrlDataBaseJobTest::~CreatePhishingUrlDataBaseJobTest()
{

}

void CreatePhishingUrlDataBaseJobTest::shouldCreateRequest_data()
{
    QTest::addColumn<QString>("databasestate");
    QTest::addColumn<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownload>("downloadtype");
    QTest::addColumn<QByteArray>("request");
    QTest::newRow("fulldownload") << QString() << WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase << QByteArray("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"listUpdateRequests\":[{\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}");
    QTest::newRow("fulldownloadwithdatabasestate") << QStringLiteral("foo") << WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase << QByteArray("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"listUpdateRequests\":[{\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}");
    QTest::newRow("partialdownloadwithdatabasestate") << QStringLiteral("foo") << WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase << QByteArray("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"listUpdateRequests\":[{\"platformType\":\"WINDOWS\",\"state\":\"foo\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}");
}

void CreatePhishingUrlDataBaseJobTest::shouldCreateRequest()
{
    QFETCH (QString, databasestate);
    QFETCH (WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownload, downloadtype);
    QFETCH (QByteArray, request);

    WebEngineViewer::CreatePhishingUrlDataBaseJob job;
    job.setDataBaseState(databasestate);
    job.setDataBaseDownloadNeeded(downloadtype);
    job.setUseCompactJson(true);
    QCOMPARE(job.jsonRequest(), request);
}


QTEST_MAIN(CreatePhishingUrlDataBaseJobTest)
