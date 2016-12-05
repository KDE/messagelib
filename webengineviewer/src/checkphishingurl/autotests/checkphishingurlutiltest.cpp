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

#include "checkphishingurlutiltest.h"
#include "../checkphishingurlutil.h"

#include <QTest>

CheckPhishingUrlUtilTest::CheckPhishingUrlUtilTest(QObject *parent)
    : QObject(parent)
{

}

CheckPhishingUrlUtilTest::~CheckPhishingUrlUtilTest()
{

}

void CheckPhishingUrlUtilTest::shouldConvertToSecond_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<double>("result");
    QTest::newRow("empty") << QString() << (double) - 1;
    QTest::newRow("test1") << QStringLiteral("459.123s") << 459.123;
    QTest::newRow("test2") << QStringLiteral("459s") << static_cast<double>(459);
}

void CheckPhishingUrlUtilTest::shouldConvertToSecond()
{
    QFETCH(QString, value);
    QFETCH(double, result);
    QCOMPARE(WebEngineViewer::CheckPhishingUrlUtil::convertToSecond(value), result);
}

void CheckPhishingUrlUtilTest::shouldCacheIsStillValid_data()
{
    QTest::addColumn<double>("second");
    QTest::addColumn<bool>("valid");
    double currentTime = QDateTime::currentDateTime().toTime_t();
    QTest::newRow("valid") << (currentTime + 2000) << true;
    QTest::newRow("invalid") << (currentTime - 2000) << false;
}

void CheckPhishingUrlUtilTest::shouldCacheIsStillValid()
{
    QFETCH(double, second);
    QFETCH(bool, valid);
    QCOMPARE(WebEngineViewer::CheckPhishingUrlUtil::cachedValueStillValid(second), valid);
}

QTEST_MAIN(CheckPhishingUrlUtilTest)
