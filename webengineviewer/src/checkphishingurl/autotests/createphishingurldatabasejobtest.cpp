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

#include "createphishingurldatabasejobtest.h"
#include "../createphishingurldatabasejob.h"
#include "../updatedatabaseinfo.h"
#include "../checkphishingurlutil.h"
#include <QSignalSpy>
#include <QTest>

extern WEBENGINEVIEWER_EXPORT bool webengineview_useCompactJson_CreatePhishingUrlDataBaseJob;

QByteArray readJsonFile(const QString &jsonFile)
{
    QFile file(QLatin1String(CHECKPHISHINGURL_DATA_DIR) + QLatin1Char('/') + jsonFile);
    file.open(QIODevice::ReadOnly);
    Q_ASSERT(file.isOpen());
    const QByteArray data = file.readAll();
    //Q_ASSERT(!data.isEmpty());
    return data;
}

QByteArray createHash(const QByteArray &ba)
{
    QByteArray b = QCryptographicHash::hash(ba, QCryptographicHash::Sha256);
    return b.toBase64();
}

CreatePhishingUrlDataBaseJobTest::CreatePhishingUrlDataBaseJobTest(QObject *parent)
    : QObject(parent)
{
    webengineview_useCompactJson_CreatePhishingUrlDataBaseJob = true;
}

CreatePhishingUrlDataBaseJobTest::~CreatePhishingUrlDataBaseJobTest()
{
}

void CreatePhishingUrlDataBaseJobTest::initTestCase()
{
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult>();
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType>();
    qRegisterMetaType<WebEngineViewer::UpdateDataBaseInfo>();
}

void CreatePhishingUrlDataBaseJobTest::shouldClearUpdateDataBaseInfo()
{
    WebEngineViewer::UpdateDataBaseInfo info;
    WebEngineViewer::UpdateDataBaseInfo info2;
    QCOMPARE(info, info2);
    info.clear();
    QCOMPARE(info, info2);

    WebEngineViewer::UpdateDataBaseInfo value;
    QVector<WebEngineViewer::Addition> additionList;
    WebEngineViewer::Addition tmp;
    tmp.prefixSize = 4;
    tmp.hashString = QByteArrayLiteral("rnGLoQ==");
    additionList.append(tmp);
    QVector<WebEngineViewer::Removal> removalList;
    WebEngineViewer::Removal tmpRemoval;
    tmpRemoval.indexes = QList<quint32>() << 0 << 2 << 4;
    removalList.append(tmpRemoval);
    value.minimumWaitDuration = QStringLiteral("593.440s");
    value.threatType = QStringLiteral("MALWARE");
    value.threatEntryType = QStringLiteral("URL");
    value.responseType = WebEngineViewer::UpdateDataBaseInfo::PartialUpdate;
    value.platformType = QStringLiteral("WINDOWS");
    value.newClientState = QStringLiteral("ChAIBRADGAEiAzAwMSiAEDABEAFGpqhd");
    value.sha256 = QByteArrayLiteral("YSgoRtsRlgHDqDA3LAhM1gegEpEzs1TjzU33vqsR8iM=");
    value.additionList = additionList;
    value.removalList = removalList;

    info = value;
    QCOMPARE(info, value);
    info2 = info;
    QCOMPARE(info, info2);
    info2.clear();
    info.clear();
    QCOMPARE(info, info2);

    WebEngineViewer::UpdateDataBaseInfo defaultValue;
    QCOMPARE(info, defaultValue);
    QCOMPARE(info2, defaultValue);
}

void CreatePhishingUrlDataBaseJobTest::shouldCreateRequest_data()
{
    QTest::addColumn<QString>("databasestate");
    QTest::addColumn<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadType>("downloadtype");
    QTest::addColumn<WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType>("contraintsCompressionType");
    QTest::addColumn<QString>("request");
    QTest::newRow("fulldownload") << QString() << WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase << WebEngineViewer::CreatePhishingUrlDataBaseJob::RawCompression
                                  << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"listUpdateRequests\":[{\"constraints\":{\"supportedCompressions\":[\"RAW\"]},\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}")
        .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps());
    QTest::newRow("fulldownloadwithdatabasestate") << QStringLiteral("foo") << WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase
                                                   << WebEngineViewer::CreatePhishingUrlDataBaseJob::RawCompression
                                                   << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"listUpdateRequests\":[{\"constraints\":{\"supportedCompressions\":[\"RAW\"]},\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}")
        .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps());
    QTest::newRow("partialdownloadwithdatabasestate") << QStringLiteral("foo") << WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase
                                                      << WebEngineViewer::CreatePhishingUrlDataBaseJob::RawCompression
                                                      << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"listUpdateRequests\":[{\"constraints\":{\"supportedCompressions\":[\"RAW\"]},\"platformType\":\"WINDOWS\",\"state\":\"foo\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}")
        .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps());
    QTest::newRow("partialdownloadwithoutdatabasestate") << QString() << WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase << WebEngineViewer::CreatePhishingUrlDataBaseJob::RawCompression
                                                         << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"listUpdateRequests\":[{\"constraints\":{\"supportedCompressions\":[\"RAW\"]},\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}")
        .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps());

    QTest::newRow("fulldownload-rice") << QString() << WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase << WebEngineViewer::CreatePhishingUrlDataBaseJob::RiceCompression
                                       << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"listUpdateRequests\":[{\"constraints\":{\"supportedCompressions\":[\"RICE\"]},\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}")
        .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps());
    QTest::newRow("fulldownloadwithdatabasestate-rice") << QStringLiteral("foo") << WebEngineViewer::CreatePhishingUrlDataBaseJob::FullDataBase
                                                        << WebEngineViewer::CreatePhishingUrlDataBaseJob::RiceCompression
                                                        << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"listUpdateRequests\":[{\"constraints\":{\"supportedCompressions\":[\"RICE\"]},\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}")
        .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps());
    QTest::newRow("partialdownloadwithdatabasestate-rice") << QStringLiteral("foo") << WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase
                                                           << WebEngineViewer::CreatePhishingUrlDataBaseJob::RiceCompression
                                                           << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"listUpdateRequests\":[{\"constraints\":{\"supportedCompressions\":[\"RICE\"]},\"platformType\":\"WINDOWS\",\"state\":\"foo\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}")
        .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps());
    QTest::newRow("partialdownloadwithoutdatabasestate-rice") << QString() << WebEngineViewer::CreatePhishingUrlDataBaseJob::UpdateDataBase
                                                              << WebEngineViewer::CreatePhishingUrlDataBaseJob::RiceCompression
                                                              << QStringLiteral(
        "{\"client\":{\"clientId\":\"KDE\",\"clientVersion\":\"%1\"},\"listUpdateRequests\":[{\"constraints\":{\"supportedCompressions\":[\"RICE\"]},\"platformType\":\"WINDOWS\",\"state\":\"\",\"threatEntryType\":\"URL\",\"threatType\":\"MALWARE\"}]}")
        .arg(WebEngineViewer::CheckPhishingUrlUtil::versionApps());
}

void CreatePhishingUrlDataBaseJobTest::shouldCreateRequest()
{
    QFETCH(QString, databasestate);
    QFETCH(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadType, downloadtype);
    QFETCH(WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType, contraintsCompressionType);
    QFETCH(QString, request);

    WebEngineViewer::CreatePhishingUrlDataBaseJob job;
    job.setDataBaseState(databasestate);
    job.setDataBaseDownloadNeeded(downloadtype);
    job.setContraintsCompressionType(contraintsCompressionType);
    QCOMPARE(job.jsonRequest(), request.toLatin1());
}

void CreatePhishingUrlDataBaseJobTest::checkRiceDeltaEncoding_data()
{
    QTest::addColumn<QByteArray>("encodingData");
    QTest::addColumn<QByteArray>("firstValue");
    QTest::addColumn<int>("numberEntries");
    QTest::addColumn<int>("riceParameter");
    QTest::addColumn<bool>("valid");
    QTest::newRow("valid") << QByteArrayLiteral("ff") << QByteArrayLiteral("AAAA") << 15 << 18 << true;
    QTest::newRow("valid1") << QByteArrayLiteral("ff") << QByteArrayLiteral("AAAA") << 0 << 0 << true;
    QTest::newRow("invalid") << QByteArrayLiteral("ff") << QByteArrayLiteral("AAAA") << 99 << 200 << false;
    QTest::newRow("invalid1") << QByteArray() << QByteArrayLiteral("AAAA") << 15 << 18 << false;
    QTest::newRow("invalid2") << QByteArrayLiteral("AAAA") << QByteArray() << 15 << 18 << false;
    QTest::newRow("invalid3") << QByteArray() << QByteArray() << 15 << 18 << false;
}

void CreatePhishingUrlDataBaseJobTest::checkRiceDeltaEncoding()
{
    QFETCH(QByteArray, encodingData);
    QFETCH(QByteArray, firstValue);
    QFETCH(int, numberEntries);
    QFETCH(int, riceParameter);
    QFETCH(bool, valid);

    WebEngineViewer::RiceDeltaEncoding a;
    a.encodingData = encodingData;
    a.firstValue = firstValue;
    a.numberEntries = numberEntries;
    a.riceParameter = riceParameter;
    QCOMPARE(a.isValid(), valid);
    WebEngineViewer::RiceDeltaEncoding b;
    b = a;
    QCOMPARE(b.isValid(), valid);
    QCOMPARE(a, b);
}

void CreatePhishingUrlDataBaseJobTest::checkAdditionElements_data()
{
    QTest::addColumn<QByteArray>("hashString");
    QTest::addColumn<int>("prefixSize");
    QTest::addColumn<WebEngineViewer::UpdateDataBaseInfo::CompressionType>("compression");
    QTest::addColumn<bool>("isValid");
    QTest::newRow("invalid") << QByteArray() << 4 << WebEngineViewer::UpdateDataBaseInfo::RawCompression << false;
    QTest::newRow("notcorrectsize") << QByteArrayLiteral("IL5HqwT2c6bltw==") << 2 << WebEngineViewer::UpdateDataBaseInfo::RawCompression << false;
    QTest::newRow("valid") << QByteArrayLiteral("IL5HqwT2c6bltw=") << 5 << WebEngineViewer::UpdateDataBaseInfo::RawCompression << true;
    QTest::newRow("invalid1") << QByteArrayLiteral("foossso") << 4 << WebEngineViewer::UpdateDataBaseInfo::RawCompression << false;
    //QByteArray b = createHash(QByteArrayLiteral("abcde"));
    //QTest::newRow("valid1") << b << 5 << true;
}

void CreatePhishingUrlDataBaseJobTest::checkAdditionElements()
{
    QFETCH(QByteArray, hashString);
    QFETCH(int, prefixSize);
    QFETCH(WebEngineViewer::UpdateDataBaseInfo::CompressionType, compression);
    QFETCH(bool, isValid);

    WebEngineViewer::Addition a;
    a.hashString = hashString;
    a.prefixSize = prefixSize;
    a.compressionType = compression;
    QCOMPARE(a.isValid(), isValid);
}

void CreatePhishingUrlDataBaseJobTest::checkRemovalElements_data()
{
    QTest::addColumn<QList<quint32> >("lst");
    QTest::addColumn<WebEngineViewer::UpdateDataBaseInfo::CompressionType>("compression");
    QTest::addColumn<bool>("isValid");
    QList<quint32> lst;
    QTest::newRow("invalid") << lst << WebEngineViewer::UpdateDataBaseInfo::RawCompression << false;
    lst << 1 << 2 << 3;
    QTest::newRow("valid") << lst << WebEngineViewer::UpdateDataBaseInfo::RawCompression << true;
}

void CreatePhishingUrlDataBaseJobTest::checkRemovalElements()
{
    QFETCH(QList<quint32>, lst);
    QFETCH(WebEngineViewer::UpdateDataBaseInfo::CompressionType, compression);
    QFETCH(bool, isValid);
    WebEngineViewer::Removal a;
    a.indexes = lst;
    a.compressionType = compression;
    QCOMPARE(a.isValid(), isValid);
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
    QByteArray hash = QByteArrayLiteral("rnGLoQ==");
    hash = QByteArray::fromBase64(hash);
    tmp.hashString = hash;
    tmp.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;
    additionList.append(tmp);
    QVector<WebEngineViewer::Removal> removalList;
    WebEngineViewer::Removal tmpRemoval;
    tmpRemoval.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;
    tmpRemoval.indexes = QList<quint32>() << 0 << 2 << 4;
    removalList.append(tmpRemoval);
    value.minimumWaitDuration = QStringLiteral("593.440s");
    value.threatType = QStringLiteral("MALWARE");
    value.threatEntryType = QStringLiteral("URL");
    value.responseType = WebEngineViewer::UpdateDataBaseInfo::PartialUpdate;
    value.platformType = QStringLiteral("WINDOWS");
    value.newClientState = QStringLiteral("ChAIBRADGAEiAzAwMSiAEDABEAFGpqhd");
    value.sha256 = QByteArrayLiteral("YSgoRtsRlgHDqDA3LAhM1gegEpEzs1TjzU33vqsR8iM=");
    value.additionList = additionList;
    value.removalList = removalList;

    QTest::newRow("test1") << QStringLiteral("test1.json") << WebEngineViewer::CreatePhishingUrlDataBaseJob::ValidData << value;

    value.clear();
    QVector<WebEngineViewer::Addition> additionList2;
    QByteArray hash1 = QByteArrayLiteral("AAAaxAAAG3QAACdhAAA");
    hash1 = QByteArray::fromBase64(hash1);
    tmp.hashString = hash1;
    tmp.prefixSize = 4;
    tmp.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;
    additionList2.append(tmp);
    QByteArray hash2 = QByteArrayLiteral("IL5HqwT2c6bltw==");
    hash2 = QByteArray::fromBase64(hash2);
    tmp.hashString = hash2;
    tmp.prefixSize = 5;
    tmp.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;
    additionList2.append(tmp);

    value.minimumWaitDuration = QStringLiteral("1786.932s");
    value.threatType = QStringLiteral("MALWARE");
    value.threatEntryType = QStringLiteral("URL");
    value.responseType = WebEngineViewer::UpdateDataBaseInfo::FullUpdate;
    value.platformType = QStringLiteral("WINDOWS");
    value.newClientState = QStringLiteral("Cg0IARAGGAEiAzAwMTABELmwARoCGAUmgN3G");
    value.sha256 = QByteArrayLiteral("ANcYWR8Umuoir+uNs1AhfxqW0iXEPDkxN6Pp2QF8dSs=");
    value.additionList = additionList2;

    QTest::newRow("test2") << QStringLiteral("test2.json") << WebEngineViewer::CreatePhishingUrlDataBaseJob::ValidData << value;
}

void CreatePhishingUrlDataBaseJobTest::shouldParseResult()
{
    QFETCH(QString, filename);
    QFETCH(WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult, parseResult);
    QFETCH(WebEngineViewer::UpdateDataBaseInfo, parseInfo);
    const QByteArray ba = readJsonFile(filename);
    WebEngineViewer::CreatePhishingUrlDataBaseJob job;
    QSignalSpy spy1(&job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::finished);
    job.parseResult(ba);
    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy1.at(0).at(1).value<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult>(), parseResult);
    QEXPECT_FAIL("test2", "Need to Investigate it", Continue);
    QCOMPARE(spy1.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>(), parseInfo);
}

QTEST_MAIN(CreatePhishingUrlDataBaseJobTest)
