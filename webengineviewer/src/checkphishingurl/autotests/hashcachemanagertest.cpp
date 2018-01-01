/*
   Copyright (C) 2016-2018 Laurent Montel <montel@kde.org>

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

#include "hashcachemanagertest.h"
#include "../hashcachemanager.h"

#include <QStandardPaths>
#include <QTest>

HashCacheManagerTest::HashCacheManagerTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

HashCacheManagerTest::~HashCacheManagerTest()
{
}

void HashCacheManagerTest::shouldBeUnknowByDefault()
{
    WebEngineViewer::HashCacheManager cache;
    QCOMPARE(cache.hashStatus(QByteArrayLiteral("foo")), WebEngineViewer::HashCacheManager::Unknown);
}

void HashCacheManagerTest::shouldAddValue_data()
{
    QTest::addColumn<QByteArray>("hash");
    QTest::addColumn<uint>("seconds");
    QTest::addColumn<WebEngineViewer::HashCacheManager::UrlStatus>("status");
    uint currentValue = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
    QTest::newRow("valid") << QByteArrayLiteral("foo") << (currentValue + 2000) << WebEngineViewer::HashCacheManager::UrlOk;
    QTest::newRow("malware1validcache") << QByteArrayLiteral("bla") << (currentValue + 2000) << WebEngineViewer::HashCacheManager::MalWare;
    QTest::newRow("malware1invalidcache") << QByteArrayLiteral("blu") << (currentValue - 2000) << WebEngineViewer::HashCacheManager::Unknown;
}

void HashCacheManagerTest::shouldAddValue()
{
    QFETCH(QByteArray, hash);
    QFETCH(uint, seconds);
    QFETCH(WebEngineViewer::HashCacheManager::UrlStatus, status);

    WebEngineViewer::HashCacheManager cache;
    cache.addHashStatus(hash, status, seconds);
    QCOMPARE(cache.hashStatus(hash), status);
    cache.clearCache();
}

QTEST_MAIN(HashCacheManagerTest)
