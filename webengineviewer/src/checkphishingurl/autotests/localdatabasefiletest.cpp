/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "localdatabasefiletest.h"
using namespace Qt::Literals::StringLiterals;

#include "../createdatabasefilejob.h"
#include "../localdatabasefile.h"
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTest>

LocalDataBaseFileTest::LocalDataBaseFileTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + u"/phishingurl"_s);
}

LocalDataBaseFileTest::~LocalDataBaseFileTest() = default;

void LocalDataBaseFileTest::shouldBeInvalidWithUnExistingFile()
{
    WebEngineViewer::LocalDataBaseFile f(u"foo"_s);
    QVERIFY(!f.isValid());
    QVERIFY(!f.fileExists());
}

void LocalDataBaseFileTest::shouldCheckHashBinaryFile_data()
{
    QTest::addColumn<QByteArray>("hash");
    QTest::addColumn<QByteArray>("resultHash");
    QTest::addColumn<bool>("found");
    QTest::newRow("nohash") << QByteArrayLiteral("foo") << QByteArrayLiteral("efgh") << true;
    QTest::newRow("foundhash") << QByteArrayLiteral("1111") << QByteArrayLiteral("1111") << true;
    QTest::newRow("foundhash1") << QByteArrayLiteral("11111") << QByteArrayLiteral("1111") << true;
    QTest::newRow("foundhash2") << QByteArrayLiteral("HGsse") << QByteArrayLiteral("54321") << true;
    QTest::newRow("foundhash3") << QByteArrayLiteral("1112") << QByteArrayLiteral("1111") << true;
}

void LocalDataBaseFileTest::shouldCheckHashBinaryFile()
{
    QFETCH(QByteArray, hash);
    QFETCH(QByteArray, resultHash);
    QFETCH(bool, found);
    WebEngineViewer::CreateDatabaseFileJob databasejob;
    const QString createDataBaseName =
        QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1StringView("/phishingurl") + QLatin1StringView("/correctBinary.db");
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

    QList<WebEngineViewer::Addition> lst;
    lst << a << b << c << d;
    info.additionList = lst;
    info.minimumWaitDuration = u"593.440s"_s;
    info.threatType = u"MALWARE"_s;
    info.threatEntryType = u"URL"_s;
    info.responseType = WebEngineViewer::UpdateDataBaseInfo::FullUpdate;
    info.platformType = u"WINDOWS"_s;
    info.newClientState = u"ChAIBRADGAEiAzAwMSiAEDABEAFGpqhd"_s;
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
    quint64 number = newFile.getUint64(4);
    QCOMPARE(number, static_cast<quint64>(9));
    int index = 4 + sizeof(quint64);
    QCOMPARE(index, 12);
    const QByteArray val = newFile.searchHash(hash);
    qDebug() << "result : " << val;
    QCOMPARE(!val.isEmpty(), found);
    if (!val.isEmpty()) {
        QCOMPARE(resultHash, val);
    }
}

QTEST_GUILESS_MAIN(LocalDataBaseFileTest)

#include "moc_localdatabasefiletest.cpp"
