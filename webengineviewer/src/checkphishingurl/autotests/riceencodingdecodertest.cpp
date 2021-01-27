/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "riceencodingdecodertest.h"
#include "../riceencodingdecoder.h"
#include <QTest>
#include <QVector>

RiceEncodingDecoderTest::RiceEncodingDecoderTest(QObject *parent)
    : QObject(parent)
{
}

RiceEncodingDecoderTest::~RiceEncodingDecoderTest()
{
}

void RiceEncodingDecoderTest::shouldDecodeRiceIndices_data()
{
    QTest::addColumn<QByteArray>("firstValue");
    QTest::addColumn<int>("riceParameter");
    QTest::addColumn<int>("numberEntries");
    QTest::addColumn<QByteArray>("encodingData");
    QTest::addColumn<QVector<quint32>>("result");
    QTest::newRow("empty") << QByteArray() << 0 << 0 << QByteArray() << QVector<quint32>();
    QVector<quint32> result;
    result << 3;
    QTest::newRow("zero value") << QByteArray("3") << 2 << 0 << QByteArray() << result;

    result.clear();
    result << 5 << 20 << 29;
    QTest::newRow("test1") << QByteArray("5") << 2 << 2 << QByteArrayLiteral("\xf7\x2") << result;

    result.clear();
    QTest::newRow("failedempty") << QByteArray("3") << 5 << 1 << QByteArray() << result;

    result.clear();
    // TODO fixme
    // QTest::newRow("failednegativeentries") << QByteArray("3") << 5 << -1 << QByteArray() << result;

    result.clear();
    // QTest::newRow("failednonpositive") << QByteArray("3") << 0 << 1 << QByteArrayLiteral("a") << result;
    result.clear();
    QTest::newRow("failednonpositive1") << QByteArray("3") << -1 << 1 << QByteArrayLiteral("a") << result;
}

void RiceEncodingDecoderTest::shouldDecodeRiceIndices()
{
    QFETCH(QByteArray, firstValue);
    QFETCH(int, riceParameter);
    QFETCH(int, numberEntries);
    QFETCH(QByteArray, encodingData);
    QFETCH(QVector<quint32>, result);
    WebEngineViewer::RiceEncodingDecoder decoding;
    WebEngineViewer::RiceDeltaEncoding deltaEncoding;
    deltaEncoding.encodingData = encodingData;
    deltaEncoding.firstValue = firstValue;
    deltaEncoding.numberEntries = numberEntries;
    deltaEncoding.riceParameter = riceParameter;
    QVector<quint32> list = decoding.decodeRiceIndiceDelta(deltaEncoding);
    QCOMPARE(list.count(), result.count());
    QCOMPARE(list, result);
}

void RiceEncodingDecoderTest::shouldDecodeRiceHashes_data()
{
    QTest::addColumn<QByteArray>("firstValue");
    QTest::addColumn<int>("riceParameter");
    QTest::addColumn<int>("numberEntries");
    QTest::addColumn<QByteArray>("encodingData");
    QTest::addColumn<QVector<quint32>>("result");
    QVector<quint32> r;
    QTest::newRow("empty") << QByteArray() << 0 << 0 << QByteArray() << r;
    r.clear();
    r = {5, 0xad934c0cu, 0x6ff67f56u, 0x81316fceu};
    QTest::newRow("test1") << QByteArrayLiteral("5") << 28 << 3 << QByteArrayLiteral("\xbf\xa8\x3f\xfb\xf\xf\x5e\x27\xe6\xc3\x1d\xc6\x38") << r;
}

void RiceEncodingDecoderTest::shouldDecodeRiceHashes()
{
    QFETCH(QByteArray, firstValue);
    QFETCH(int, riceParameter);
    QFETCH(int, numberEntries);
    QFETCH(QByteArray, encodingData);
    QFETCH(QVector<quint32>, result);
    WebEngineViewer::RiceEncodingDecoder decoding;
    WebEngineViewer::RiceDeltaEncoding deltaEncoding;
    deltaEncoding.encodingData = encodingData;
    deltaEncoding.firstValue = firstValue;
    deltaEncoding.numberEntries = numberEntries;
    deltaEncoding.riceParameter = riceParameter;
    const QVector<quint32> hash = decoding.decodeRiceHashesDelta(deltaEncoding);
    qDebug() << " hash " << hash << " 0xad934c0cu" << 0xad934c0cu;
    QCOMPARE(hash, result);
}

QTEST_GUILESS_MAIN(RiceEncodingDecoderTest)
