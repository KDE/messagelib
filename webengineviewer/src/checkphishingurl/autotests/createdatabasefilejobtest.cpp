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

#include "createdatabasefilejobtest.h"
#include "../createdatabasefilejob.h"
#include "../createphishingurldatabasejob.h"
#include "../localdatabasefile.h"
#include <QStandardPaths>

#include <QSignalSpy>
#include <QTest>
#include <QDebug>

QByteArray readJsonFile(const QString &jsonFile)
{
    QFile file(QLatin1String(CHECKPHISHINGURL_DATA_DIR) + QLatin1Char('/') + jsonFile);
    file.open(QIODevice::ReadOnly);
    Q_ASSERT(file.isOpen());
    const QByteArray data = file.readAll();
    Q_ASSERT(!data.isEmpty());
    return data;
}

CreateDatabaseFileJobTest::CreateDatabaseFileJobTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

CreateDatabaseFileJobTest::~CreateDatabaseFileJobTest()
{

}

void CreateDatabaseFileJobTest::initTestcase()
{
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult>();
    qRegisterMetaType<WebEngineViewer::CreatePhishingUrlDataBaseJob::ContraintsCompressionType>();
    qRegisterMetaType<WebEngineViewer::UpdateDataBaseInfo>();
}

void CreateDatabaseFileJobTest::shouldHaveDefaultValue()
{
    WebEngineViewer::CreateDatabaseFileJob job;
    QVERIFY(!job.canStart());
}

void CreateDatabaseFileJobTest::shouldCreateFile_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<quint64>("numberOfElement");
    QTest::addColumn<bool>("success");
    QTest::newRow("correctdatabase") << QStringLiteral("current.json") << static_cast<quint64>(580600) << true;
    QTest::newRow("correctdatabase2") << QStringLiteral("newdatabase2.json") << static_cast<quint64>(579416) << true;
    QTest::newRow("incorrectdatabase") << QStringLiteral("incorrectdatabase2.json") << static_cast<quint64>(0) << false;
}

void CreateDatabaseFileJobTest::shouldCreateFile()
{
    QFETCH(QString, filename);
    QFETCH(quint64, numberOfElement);
    QFETCH(bool, success);

    const QByteArray ba = readJsonFile(filename);
    WebEngineViewer::CreatePhishingUrlDataBaseJob job;
    QSignalSpy spy1(&job, SIGNAL(finished(WebEngineViewer::UpdateDataBaseInfo,WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)));
    job.parseResult(ba);
    QCOMPARE(spy1.count(), 1);
    const WebEngineViewer::UpdateDataBaseInfo info = spy1.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>();
    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/phishingurl") + QStringLiteral("/test.db");
    qDebug() << " new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);
    databasejob.setUpdateDataBaseInfo(info);

    QSignalSpy spy2(&databasejob, SIGNAL(finished(bool)));
    databasejob.start();
    QCOMPARE(spy2.count(), 1);
    bool successCreateDataBase = spy2.at(0).at(0).toBool();
    QCOMPARE(successCreateDataBase, success);

    WebEngineViewer::LocalDataBaseFile newFile(createDataBaseName);
    QVERIFY(newFile.isValid());
    QCOMPARE(newFile.getUint16(0), static_cast<quint16>(1));
    QCOMPARE(newFile.getUint16(2), static_cast<quint16>(0));
    if (success) {
        QCOMPARE(newFile.getUint64(4), numberOfElement);
    }
}

void CreateDatabaseFileJobTest::shouldRemoveElementInDataBase_data()
{
    QTest::addColumn<QList<int> >("listElementToRemove");
    QTest::addColumn<QByteArray>("newssha");
    QTest::addColumn<bool>("success");
    QList<int> r = { 2, 3, 4};
    QTest::newRow("correctdatabase") << r << QByteArrayLiteral("yTnyjAgIFeS6Cv+b4IJHngYbdvp5uz1bx9V4el5CyeE=") << true;
    r = { 3, 2, 4};
    QTest::newRow("correctdatabaseotherorder") << r << QByteArrayLiteral("yTnyjAgIFeS6Cv+b4IJHngYbdvp5uz1bx9V4el5CyeE=") << true;

    r = { 4, 2, 3};
    QTest::newRow("correctdatabaseotherorder2") << r << QByteArrayLiteral("yTnyjAgIFeS6Cv+b4IJHngYbdvp5uz1bx9V4el5CyeE=") << true;
}

void CreateDatabaseFileJobTest::shouldRemoveElementInDataBase()
{
    QFETCH(QList<int>, listElementToRemove);
    QFETCH(QByteArray, newssha);
    QFETCH(bool, success);

    // Proof of checksum validity using python:
    // >>> import hashlib
    // >>> m = hashlib.sha256()
    // >>> m.update("----11112222254321abcdabcdebbbbbcdefefgh")
    // >>> m.digest()
    // "\xbc\xb3\xedk\xe3x\xd1(\xa9\xedz7]"
    // "x\x18\xbdn]\xa5\xa8R\xf7\xab\xcf\xc1\xa3\xa3\xc5Z,\xa6o"

    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/phishingurl") + QStringLiteral("/correctBinary.db");
    qDebug() << " new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);

    WebEngineViewer::UpdateDataBaseInfo info;

    WebEngineViewer::Addition a;
    a.hashString = QByteArray("----1111bbbb");
    a.prefixSize = 4;

    WebEngineViewer::Addition b;
    b.hashString = QByteArray("abcdefgh");
    b.prefixSize = 4;

    WebEngineViewer::Addition c;
    c.hashString = QByteArray("54321abcde");
    c.prefixSize = 5;

    WebEngineViewer::Addition d;
    d.hashString = QByteArray("22222bcdef");
    d.prefixSize = 5;

    QVector<WebEngineViewer::Addition> lst;
    lst << a << b << c << d;
    info.additionList = lst;
    info.minimumWaitDuration = QStringLiteral("593.440s");
    info.threatType = QStringLiteral("MALWARE");
    info.threatEntryType = QStringLiteral("URL");
    info.responseType = WebEngineViewer::UpdateDataBaseInfo::FullUpdate;
    info.platformType = QStringLiteral("WINDOWS");
    info.newClientState = QStringLiteral("ChAIBRADGAEiAzAwMSiAEDABEAFGpqhd");
    info.sha256 = QByteArrayLiteral("vLPta+N40Sip7Xo3XXgYvW5dpahS96vPwaOjxVospm8=");

    databasejob.setUpdateDataBaseInfo(info);

    QSignalSpy spy2(&databasejob, SIGNAL(finished(bool)));
    databasejob.start();
    QCOMPARE(spy2.count(), 1);
    bool successCreateDataBase = spy2.at(0).at(0).toBool();
    QVERIFY(successCreateDataBase);

    WebEngineViewer::LocalDataBaseFile newFile(createDataBaseName);
    QVERIFY(newFile.isValid());
    QCOMPARE(newFile.getUint16(0), static_cast<quint16>(1));
    QCOMPARE(newFile.getUint16(2), static_cast<quint16>(0));
    QCOMPARE(newFile.getUint64(4), static_cast<quint64>(9));
    int index = 4 + sizeof(quint64);
    QList<QByteArray> storageData;
    storageData << QByteArrayLiteral("----");
    storageData << QByteArrayLiteral("1111");
    storageData << QByteArrayLiteral("22222");
    storageData << QByteArrayLiteral("54321");
    storageData << QByteArrayLiteral("abcd");
    storageData << QByteArrayLiteral("abcde");
    storageData << QByteArrayLiteral("bbbb");
    storageData << QByteArrayLiteral("bcdef");
    storageData << QByteArrayLiteral("efgh");

    for (int i = 0; i < 9; ++i) {
        quint64 value = newFile.getUint64(index);
        //qDebug() << "char "<< newFile.getCharStar(value);
        QCOMPARE(storageData.at(i), QByteArray(newFile.getCharStar(value)));
        index += sizeof(quint64);
    }
    const QVector<WebEngineViewer::Addition> lstInfo = newFile.extractAllInfo();
    QCOMPARE(lstInfo.count(), 9);
    for (int i = 0; i < 9; i++) {
        QCOMPARE(lstInfo.at(i).hashString, storageData.at(i));
        QCOMPARE(lstInfo.at(i).prefixSize, lstInfo.at(i).hashString.size());
    }

    //Before
    //storageData << QByteArrayLiteral("----");
    //storageData << QByteArrayLiteral("1111");
    //storageData << QByteArrayLiteral("22222");
    //storageData << QByteArrayLiteral("54321");
    //storageData << QByteArrayLiteral("abcd");
    //storageData << QByteArrayLiteral("abcde");
    //storageData << QByteArrayLiteral("bbbb");
    //storageData << QByteArrayLiteral("bcdef");
    //storageData << QByteArrayLiteral("efgh");

    //TODO remove items.
    WebEngineViewer::UpdateDataBaseInfo updateinfo;

    // we will remove QByteArrayLiteral("22222"); QByteArrayLiteral("54321"); QByteArrayLiteral("abcd");
    WebEngineViewer::Removal r;
    r.indexes = listElementToRemove;

    // Proof of checksum validity using python:
    // >>> import hashlib
    // >>> m = hashlib.sha256()
    // >>> m.update("----1111abcdebbbbbcdefefgh")
    // >>> m.digest()
    // '\xc99\xf2\x8c\x08\x08\x15\xe4\xba\n\xff\x9b\xe0\x82G\x9e\x06\x1bv\xfay\xbb=[\xc7\xd5xz^B\xc9\xe1'
    // >>> import base64
    // >>> encoded = base64.b64encode(m.digest())
    // >>> encoded
    // 'yTnyjAgIFeS6Cv+b4IJHngYbdvp5uz1bx9V4el5CyeE='

    QVector<WebEngineViewer::Removal> lstRemovals;
    lstRemovals << r;
    updateinfo.removalList = lstRemovals;
    updateinfo.minimumWaitDuration = QStringLiteral("593.440s");
    updateinfo.threatType = QStringLiteral("MALWARE");
    updateinfo.threatEntryType = QStringLiteral("URL");
    updateinfo.responseType = WebEngineViewer::UpdateDataBaseInfo::PartialUpdate;
    updateinfo.platformType = QStringLiteral("WINDOWS");
    updateinfo.newClientState = QStringLiteral("ChAIBRADGAEiAzAwMSiAEDABEAFGpqhd");
    updateinfo.sha256 = /*QByteArrayLiteral("yTnyjAgIFeS6Cv+b4IJHngYbdvp5uz1bx9V4el5CyeE=")*/ newssha;

    WebEngineViewer::CreateDatabaseFileJob updateDatabasejob;
    qDebug() << " new filename " << createDataBaseName;
    updateDatabasejob.setFileName(createDataBaseName);

    updateDatabasejob.setUpdateDataBaseInfo(updateinfo);

    QSignalSpy spy3(&updateDatabasejob, SIGNAL(finished(bool)));
    updateDatabasejob.start();
    QCOMPARE(spy3.count(), 1);
    successCreateDataBase = spy3.at(0).at(0).toBool();
    QCOMPARE(successCreateDataBase, success);

}

void CreateDatabaseFileJobTest::shouldCreateCorrectBinaryFile()
{
    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/phishingurl") + QStringLiteral("/correctBinary.db");
    qDebug() << " new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);

    WebEngineViewer::UpdateDataBaseInfo info;

    WebEngineViewer::Addition a;
    a.hashString = QByteArray("----1111bbbb");
    a.prefixSize = 4;

    WebEngineViewer::Addition b;
    b.hashString = QByteArray("abcdefgh");
    b.prefixSize = 4;

    WebEngineViewer::Addition c;
    c.hashString = QByteArray("54321abcde");
    c.prefixSize = 5;

    WebEngineViewer::Addition d;
    d.hashString = QByteArray("22222bcdef");
    d.prefixSize = 5;

    QVector<WebEngineViewer::Addition> lst;
    lst << a << b << c << d;
    info.additionList = lst;
    info.minimumWaitDuration = QStringLiteral("593.440s");
    info.threatType = QStringLiteral("MALWARE");
    info.threatEntryType = QStringLiteral("URL");
    info.responseType = WebEngineViewer::UpdateDataBaseInfo::FullUpdate;
    info.platformType = QStringLiteral("WINDOWS");
    info.newClientState = QStringLiteral("ChAIBRADGAEiAzAwMSiAEDABEAFGpqhd");
    info.sha256 = QByteArrayLiteral("vLPta+N40Sip7Xo3XXgYvW5dpahS96vPwaOjxVospm8=");

    databasejob.setUpdateDataBaseInfo(info);

    QSignalSpy spy2(&databasejob, SIGNAL(finished(bool)));
    databasejob.start();
    QCOMPARE(spy2.count(), 1);
    bool successCreateDataBase = spy2.at(0).at(0).toBool();
    QVERIFY(successCreateDataBase);

    WebEngineViewer::LocalDataBaseFile newFile(createDataBaseName);
    QVERIFY(newFile.isValid());
    QCOMPARE(newFile.getUint16(0), static_cast<quint16>(1));
    QCOMPARE(newFile.getUint16(2), static_cast<quint16>(0));
    QCOMPARE(newFile.getUint64(4), static_cast<quint64>(9));
    int index = 4 + sizeof(quint64);
    QList<QByteArray> storageData;
    storageData << QByteArrayLiteral("----");
    storageData << QByteArrayLiteral("1111");
    storageData << QByteArrayLiteral("22222");
    storageData << QByteArrayLiteral("54321");
    storageData << QByteArrayLiteral("abcd");
    storageData << QByteArrayLiteral("abcde");
    storageData << QByteArrayLiteral("bbbb");
    storageData << QByteArrayLiteral("bcdef");
    storageData << QByteArrayLiteral("efgh");

    for (int i = 0; i < 9; ++i) {
        quint64 value = newFile.getUint64(index);
        //qDebug() << "char "<< newFile.getCharStar(value);
        QCOMPARE(storageData.at(i), QByteArray(newFile.getCharStar(value)));
        index += sizeof(quint64);
    }
    const QVector<WebEngineViewer::Addition> lstInfo = newFile.extractAllInfo();
    QCOMPARE(lstInfo.count(), 9);
    for (int i = 0; i < 9; i++) {
        QCOMPARE(lstInfo.at(i).hashString, storageData.at(i));
        QCOMPARE(lstInfo.at(i).prefixSize, lstInfo.at(i).hashString.size());
    }
}

void CreateDatabaseFileJobTest::shouldUpdateDataBase()
{
    QString firstFilename = QStringLiteral("current.json");
    const QByteArray ba = readJsonFile(firstFilename);
    WebEngineViewer::CreatePhishingUrlDataBaseJob job;
    QSignalSpy spy1(&job, SIGNAL(finished(WebEngineViewer::UpdateDataBaseInfo,WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)));
    job.parseResult(ba);
    QCOMPARE(spy1.count(), 1);
    const WebEngineViewer::UpdateDataBaseInfo info = spy1.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>();
    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/phishingurl") + QStringLiteral("/update.db");
    qDebug() << " new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);
    databasejob.setUpdateDataBaseInfo(info);

    QSignalSpy spy2(&databasejob, SIGNAL(finished(bool)));
    databasejob.start();
    QCOMPARE(spy2.count(), 1);
    bool successCreateDataBase = spy2.at(0).at(0).toBool();
    QVERIFY(successCreateDataBase);

    WebEngineViewer::LocalDataBaseFile newFile(createDataBaseName);
    QVERIFY(newFile.isValid());
    QCOMPARE(newFile.getUint16(0), static_cast<quint16>(1));
    QCOMPARE(newFile.getUint16(2), static_cast<quint16>(0));
    QCOMPARE(newFile.getUint64(4), static_cast<quint64>(580600));
    newFile.close();

    QString updateFilename = QStringLiteral("partial_download2.json");
    const QByteArray baUpdate = readJsonFile(updateFilename);

    WebEngineViewer::CreatePhishingUrlDataBaseJob jobUpdate;
    QSignalSpy spy3(&jobUpdate, SIGNAL(finished(WebEngineViewer::UpdateDataBaseInfo,WebEngineViewer::CreatePhishingUrlDataBaseJob::DataBaseDownloadResult)));
    jobUpdate.parseResult(baUpdate);
    QCOMPARE(spy3.count(), 1);

    const WebEngineViewer::UpdateDataBaseInfo infoUpdate = spy3.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>();
    QCOMPARE(infoUpdate.responseType, WebEngineViewer::UpdateDataBaseInfo::PartialUpdate);

    WebEngineViewer::CreateDatabaseFileJob databasejob2;
    databasejob2.setFileName(createDataBaseName);
    databasejob2.setUpdateDataBaseInfo(infoUpdate);

    QSignalSpy spy4(&databasejob2, SIGNAL(finished(bool)));
    databasejob2.start();
    QCOMPARE(spy4.count(), 1);
    successCreateDataBase = spy4.at(0).at(0).toBool();
    QVERIFY(successCreateDataBase);

}

QTEST_MAIN(CreateDatabaseFileJobTest)
