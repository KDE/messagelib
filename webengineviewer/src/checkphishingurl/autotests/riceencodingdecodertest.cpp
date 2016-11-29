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
    //TODO add more
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
    QCOMPARE(list, result);
}

void RiceEncodingDecoderTest::shouldDecodeRiceHashes_data()
{
    QTest::addColumn<QByteArray>("firstValue");
    QTest::addColumn<int>("riceParameter");
    QTest::addColumn<int>("numberEntries");
    QTest::addColumn<QByteArray>("encodingData");
    QTest::addColumn<QByteArray >("result");
    QTest::newRow("empty") << QByteArray() << 0 << 0 << QByteArray() << QByteArray();
    //TODO add more
}

void RiceEncodingDecoderTest::shouldDecodeRiceHashes()
{
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
}

QTEST_MAIN(RiceEncodingDecoderTest)
