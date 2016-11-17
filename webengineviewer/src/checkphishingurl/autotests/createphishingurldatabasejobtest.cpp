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

#include <QSignalSpy>
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

void CreatePhishingUrlDataBaseJobTest::initTestcase()
{
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult>();
    qRegisterMetaType<WebEngineViewer::UpdateDataBaseInfo>();
}

void CreatePhishingUrlDataBaseJobTest::shouldCreateRequest_data()
{
    QTest::addColumn<QString>("databasestate");
    QTest::addColumn<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownload>("downloadtype");
    QTest::addColumn<QByteArray>("request");
    QTest::newRow("fulldownload") << QString() << WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase << QByteArray("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"listUpdateRequests\":[{\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}");
    QTest::newRow("fulldownloadwithdatabasestate") << QStringLiteral("foo") << WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase << QByteArray("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"listUpdateRequests\":[{\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}");
    QTest::newRow("partialdownloadwithdatabasestate") << QStringLiteral("foo") << WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase << QByteArray("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"listUpdateRequests\":[{\"platformType\":\"WINDOWS\",\"state\":\"foo\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}");
    QTest::newRow("partialdownloadwithoutdatabasestate") << QString() << WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase << QByteArray("{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"5.4.0\"},\"listUpdateRequests\":[{\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}");
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

void CreatePhishingUrlDataBaseJobTest::shouldParseResult_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult>("parseResult");
    QTest::addColumn<WebEngineViewer::UpdateDataBaseInfo>("parseInfo");
    QTest::newRow("emptydocument") << QStringLiteral("empty.json") << WebEngineViewer::CreatePhishingUrlDataBaseJob::InvalidData << WebEngineViewer::UpdateDataBaseInfo();
    QTest::newRow("emptydocument2") << QStringLiteral("empty2.json") << WebEngineViewer::CreatePhishingUrlDataBaseJob::InvalidData << WebEngineViewer::UpdateDataBaseInfo();
    WebEngineViewer::UpdateDataBaseInfo value;
    QVector<WebEngineViewer::Addition> additionList;
    WebEngineViewer::Addition tmp;
    tmp.prefixSize = 4;
    tmp.hashString = QByteArrayLiteral("rnGLoQ==");
    additionList.append(tmp);
    QVector<WebEngineViewer::Removal> removalList;
    WebEngineViewer::Removal tmpRemoval;
    removalList.append(tmpRemoval);
    value.minimumWaitDuration = QStringLiteral("593.440s");
    value.threatType = QStringLiteral("MALWARE");
    value.threatEntryType = QStringLiteral("URL");
    value.responseType = QStringLiteral("PARTIAL_UPDATE");
    value.platformType = QStringLiteral("WINDOWS");
    value.newClientState = QStringLiteral("ChAIBRADGAEiAzAwMSiAEDABEAFGpqhd");
    value.sha256 = QStringLiteral("YSgoRtsRlgHDqDA3LAhM1gegEpEzs1TjzU33vqsR8iM=");
    value.additionList = additionList;
    //value.removalList = removalList;

    QTest::newRow("test1") << QStringLiteral("test1.json") << WebEngineViewer::CreatePhishingUrlDataBaseJob::ValidData << value;
}

void CreatePhishingUrlDataBaseJobTest::shouldParseResult()
{
    QFETCH (QString, filename);
    QFETCH(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult, parseResult);
    QFETCH(WebEngineViewer::UpdateDataBaseInfo, parseInfo);
    const QByteArray ba = readJsonFile(filename);
    WebEngineViewer::CreatePhishingUrlDataBaseJob job;
    QSignalSpy spy1(&job, SIGNAL(finished(WebEngineViewer::UpdateDataBaseInfo,WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)));
    job.parseResult(ba);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy1.at(0).at(1).value<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult>(), parseResult);
    QCOMPARE(spy1.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>(), parseInfo);
}


QTEST_MAIN(CreatePhishingUrlDataBaseJobTest)
