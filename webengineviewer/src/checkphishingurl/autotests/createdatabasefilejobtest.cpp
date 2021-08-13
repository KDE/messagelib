/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "createdatabasefilejobtest.h"
#include "../createdatabasefilejob.h"
#include "../createphishingurldatabasejob.h"
#include "../localdatabasefile.h"
#include <QStandardPaths>

#include <QDebug>
#include <QSignalSpy>
#include <QTest>
Q_DECLARE_METATYPE(QVector<WebEngineViewer::Addition>)
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
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/phishingurl"));
}

CreateDatabaseFileJobTest::~CreateDatabaseFileJobTest()
{
}

void CreateDatabaseFileJobTest::initTestCase()
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
    QSignalSpy spy1(&job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::finished);
    job.parseResult(ba);
    QCOMPARE(spy1.count(), 1);
    const auto info = spy1.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>();
    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/phishingurl") + QLatin1String("/test.db");
    qDebug() << " new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);
    databasejob.setUpdateDataBaseInfo(info);

    QSignalSpy spy2(&databasejob, &WebEngineViewer::CreateDatabaseFileJob::finished);
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
    QTest::addColumn<QList<quint32>>("listElementToRemove");
    QTest::addColumn<QVector<WebEngineViewer::Addition>>("listElementToAdd");
    QTest::addColumn<QByteArray>("newssha");
    QTest::addColumn<bool>("success");
    QVector<WebEngineViewer::Addition> lstAdditions;
    QList<quint32> r = {2, 3, 4};
    QTest::newRow("correctdatabase") << r << lstAdditions << QByteArrayLiteral("yTnyjAgIFeS6Cv+b4IJHngYbdvp5uz1bx9V4el5CyeE=") << true;
    r = {3, 2, 4};
    QTest::newRow("correctdatabaseotherorder") << r << lstAdditions << QByteArrayLiteral("yTnyjAgIFeS6Cv+b4IJHngYbdvp5uz1bx9V4el5CyeE=") << true;

    r = {4, 2, 3};
    QTest::newRow("correctdatabaseotherorder2") << r << lstAdditions << QByteArrayLiteral("yTnyjAgIFeS6Cv+b4IJHngYbdvp5uz1bx9V4el5CyeE=") << true;

    //    >>> import hashlib
    //    >>> m = hashlib.sha256()
    //    >>> m.update("111154321abcdabcdebbbbbcdef")
    //    >>> m.digest()
    //    '\x81\xdd9\xe3\xae\x94s\xfd\x16o\\\xcea \xb7\xbc\x1b+R\nN\x05o\xfe\xeeWY\x7f\x8a\xcb\xbeN'
    //    >>> import base64
    //    >>> encoded = base64.b64encode(m.digest())
    //    >>> encoded
    //    'gd05466Uc/0Wb1zOYSC3vBsrUgpOBW/+7ldZf4rLvk4='
    //    >>>

    r = {0, 2, 8};
    QTest::newRow("correctdatabaseotherorder3") << r << lstAdditions << QByteArrayLiteral("gd05466Uc/0Wb1zOYSC3vBsrUgpOBW/+7ldZf4rLvk4=") << true;

    r = {0, 2, 8};

    WebEngineViewer::Addition c;
    c.hashString = QByteArray("mnopqrst");
    c.prefixSize = 4;
    c.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

    WebEngineViewer::Addition b;
    b.hashString = QByteArray("uvwx");
    b.prefixSize = 4;
    b.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

    lstAdditions << c << b;

    //    >>> import hashlib
    //    >>> m = hashlib.sha256()
    //    >>> m.update("111154321abcdabcdebbbbbcdefmnopqrstuvwx")
    //    >>> m.digest()
    //    '\n\xae\xe2\xe0!\x8f\xa4\x05N\x89,\xdcJ*\xbe\x85\xa1Q\xc3\x9c\xc8}j\x83*s\xd5L&\xbe\xfbh'
    //    >>> import base64
    //    >>> encoded = base64.b64encode(m.digest())
    //    >>> encoded
    //    'Cq7i4CGPpAVOiSzcSiq+haFRw5zIfWqDKnPVTCa++2g='

    // m.update("111154321abcdabcdebbbbbcdefmnopqrstuvwx");

    QTest::newRow("correctdatabaseotherorderwithadditions") << r << lstAdditions << QByteArrayLiteral("Cq7i4CGPpAVOiSzcSiq+haFRw5zIfWqDKnPVTCa++2g=") << true;
}

void CreateDatabaseFileJobTest::shouldRemoveElementInDataBase()
{
    QFETCH(QList<quint32>, listElementToRemove);
    QFETCH(QVector<WebEngineViewer::Addition>, listElementToAdd);
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
    const QString createDataBaseName =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/phishingurl") + QLatin1String("/correctBinary.db");
    qDebug() << " new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);

    WebEngineViewer::UpdateDataBaseInfo info;

    WebEngineViewer::Addition a;
    a.hashString = QByteArray("----1111bbbb");
    a.prefixSize = 4;
    a.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

    WebEngineViewer::Addition b;
    b.hashString = QByteArray("abcdefgh");
    b.prefixSize = 4;
    b.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

    WebEngineViewer::Addition c;
    c.hashString = QByteArray("54321abcde");
    c.prefixSize = 5;
    c.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

    WebEngineViewer::Addition d;
    d.hashString = QByteArray("22222bcdef");
    d.prefixSize = 5;
    d.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

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

    QSignalSpy spy2(&databasejob, &WebEngineViewer::CreateDatabaseFileJob::finished);
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
        // qDebug() << "char "<< newFile.getCharStar(value);
        QCOMPARE(storageData.at(i), QByteArray(newFile.getCharStar(value)));
        index += sizeof(quint64);
    }
    const QVector<WebEngineViewer::Addition> lstInfo = newFile.extractAllInfo();
    QCOMPARE(lstInfo.count(), 9);
    for (int i = 0; i < 9; i++) {
        QCOMPARE(lstInfo.at(i).hashString, storageData.at(i));
        QCOMPARE(lstInfo.at(i).prefixSize, lstInfo.at(i).hashString.size());
    }

    // Before
    // storageData << QByteArrayLiteral("----");
    // storageData << QByteArrayLiteral("1111");
    // storageData << QByteArrayLiteral("22222");
    // storageData << QByteArrayLiteral("54321");
    // storageData << QByteArrayLiteral("abcd");
    // storageData << QByteArrayLiteral("abcde");
    // storageData << QByteArrayLiteral("bbbb");
    // storageData << QByteArrayLiteral("bcdef");
    // storageData << QByteArrayLiteral("efgh");

    // TODO remove items.
    WebEngineViewer::UpdateDataBaseInfo updateinfo;

    // we will remove QByteArrayLiteral("22222"); QByteArrayLiteral("54321"); QByteArrayLiteral("abcd");
    WebEngineViewer::Removal r;
    r.indexes = listElementToRemove;
    r.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

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
    updateinfo.additionList = listElementToAdd;
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

    QSignalSpy spy3(&updateDatabasejob, &WebEngineViewer::CreateDatabaseFileJob::finished);
    updateDatabasejob.start();
    QCOMPARE(spy3.count(), 1);
    successCreateDataBase = spy3.at(0).at(0).toBool();
    QCOMPARE(successCreateDataBase, success);
}

void CreateDatabaseFileJobTest::shouldCreateCorrectBinaryFile()
{
    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/phishingurl") + QLatin1String("/correctBinary.db");
    qDebug() << " new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);

    WebEngineViewer::UpdateDataBaseInfo info;

    WebEngineViewer::Addition a;
    a.hashString = QByteArray("----1111bbbb");
    a.prefixSize = 4;
    a.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

    WebEngineViewer::Addition b;
    b.hashString = QByteArray("abcdefgh");
    b.prefixSize = 4;
    b.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

    WebEngineViewer::Addition c;
    c.hashString = QByteArray("54321abcde");
    c.prefixSize = 5;
    c.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

    WebEngineViewer::Addition d;
    d.hashString = QByteArray("22222bcdef");
    d.prefixSize = 5;
    d.compressionType = WebEngineViewer::UpdateDataBaseInfo::RawCompression;

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

    QSignalSpy spy2(&databasejob, &WebEngineViewer::CreateDatabaseFileJob::finished);
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
        // qDebug() << "char "<< newFile.getCharStar(value);
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
    QString firstFilename = QStringLiteral("newdatabase2.json");
    const QByteArray ba = readJsonFile(firstFilename);
    WebEngineViewer::CreatePhishingUrlDataBaseJob job;
    QSignalSpy spy1(&job, &WebEngineViewer::CreatePhishingUrlDataBaseJob::finished);
    job.parseResult(ba);
    QCOMPARE(spy1.count(), 1);
    const auto info = spy1.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>();
    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1String("/phishingurl") + QLatin1String("/update.db");
    // qDebug() << " new filename " << createDataBaseName;
    databasejob.setFileName(createDataBaseName);
    databasejob.setUpdateDataBaseInfo(info);

    QSignalSpy spy2(&databasejob, &WebEngineViewer::CreateDatabaseFileJob::finished);
    databasejob.start();
    QCOMPARE(spy2.count(), 1);
    bool successCreateDataBase = spy2.at(0).at(0).toBool();
    QVERIFY(successCreateDataBase);

    WebEngineViewer::LocalDataBaseFile newFile(createDataBaseName);
    QVERIFY(newFile.isValid());
    QCOMPARE(newFile.getUint16(0), static_cast<quint16>(1));
    QCOMPARE(newFile.getUint16(2), static_cast<quint16>(0));
    QCOMPARE(newFile.getUint64(4), static_cast<quint64>(579416));
    newFile.close();

    QString updateFilename = QStringLiteral("partial_download3.json");
    const QByteArray baUpdate = readJsonFile(updateFilename);

    WebEngineViewer::CreatePhishingUrlDataBaseJob jobUpdate;
    QSignalSpy spy3(&jobUpdate, &WebEngineViewer::CreatePhishingUrlDataBaseJob::finished);
    jobUpdate.parseResult(baUpdate);
    QCOMPARE(spy3.count(), 1);

    const auto infoUpdate = spy3.at(0).at(0).value<WebEngineViewer::UpdateDataBaseInfo>();
    QCOMPARE(infoUpdate.responseType, WebEngineViewer::UpdateDataBaseInfo::PartialUpdate);

    WebEngineViewer::CreateDatabaseFileJob databasejob2;
    databasejob2.setFileName(createDataBaseName);
    databasejob2.setUpdateDataBaseInfo(infoUpdate);

    QSignalSpy spy4(&databasejob2, &WebEngineViewer::CreateDatabaseFileJob::finished);
    databasejob2.start();
    QCOMPARE(spy4.count(), 1);
    successCreateDataBase = spy4.at(0).at(0).toBool();
    QEXPECT_FAIL("", "Expected a success but not", Continue);
    QVERIFY(successCreateDataBase);
}

QTEST_MAIN(CreateDatabaseFileJobTest)
