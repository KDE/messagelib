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

#include "riceencodingdecodertest.h"
#include "../riceencodingdecoder.h"
#include <QTest>

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
    QTest::addColumn<QList<int> >("result");
    QTest::newRow("empty") << QByteArray() << 0 << 0 << QByteArray() << QList<int>();
    QList<int> result;
    result << 3;
    QTest::newRow("zero value") << QByteArray("3") << 2 << 0 << QByteArray() << result;

    result.clear();
    result << 5 << 20 << 29;
    QTest::newRow("test1") << QByteArray("5") << 2 << 2 << QByteArrayLiteral("\xf7\x2") << result;

    result.clear();
    QTest::newRow("failedempty") << QByteArray("3") << 5 << 1 << QByteArray() << result;

    result.clear();
    //TODO fixme
    //QTest::newRow("failednegativeentries") << QByteArray("3") << 5 << -1 << QByteArray() << result;

    result.clear();
    QTest::newRow("failednonpositive") << QByteArray("3") << 0 << 1 << QByteArrayLiteral("a") << result;
    QTest::newRow("failednonpositive1") << QByteArray("3") << -1 << 1 << QByteArrayLiteral("a") << result;
}

void RiceEncodingDecoderTest::shouldDecodeRiceIndices()
{
    QFETCH(QByteArray, firstValue);
    QFETCH(int, riceParameter);
    QFETCH(int, numberEntries);
    QFETCH(QByteArray, encodingData);
    QFETCH(QList<int>, result);
    WebEngineViewer::RiceEncodingDecoder decoding;
    WebEngineViewer::RiceDeltaEncoding deltaEncoding;
    deltaEncoding.encodingData = encodingData;
    deltaEncoding.firstValue = firstValue;
    deltaEncoding.numberEntries = numberEntries;
    deltaEncoding.riceParameter = riceParameter;
    QList<int> list = decoding.decodeRiceIndiceDelta(deltaEncoding);
    QCOMPARE(list.count(), result.count());
    QCOMPARE(list, result);
}

void RiceEncodingDecoderTest::shouldDecodeRiceHashes_data()
{
    //TODO
#if 0
    QTest::addColumn<QByteArray>("firstValue");
    QTest::addColumn<int>("riceParameter");
    QTest::addColumn<int>("numberEntries");
    QTest::addColumn<QByteArray>("encodingData");
    QTest::addColumn<QByteArray >("result");
    QTest::newRow("empty") << QByteArray() << 0 << 0 << QByteArray() << QByteArray();
    //TODO add more
#endif
}

void RiceEncodingDecoderTest::shouldDecodeRiceHashes()
{
#if 0
    QFETCH(QByteArray, firstValue);
    QFETCH(int, riceParameter);
    QFETCH(int, numberEntries);
    QFETCH(QByteArray, encodingData);
    QFETCH(QByteArray, result);
    WebEngineViewer::RiceEncodingDecoder decoding;
    WebEngineViewer::RiceDeltaEncoding deltaEncoding;
    deltaEncoding.encodingData = encodingData;
    deltaEncoding.firstValue = firstValue;
    deltaEncoding.numberEntries = numberEntries;
    deltaEncoding.riceParameter = riceParameter;
    const QByteArray hash = decoding.decodeRiceHashesDelta(deltaEncoding);
    QCOMPARE(hash, result);
#endif
}

QTEST_MAIN(RiceEncodingDecoderTest)
