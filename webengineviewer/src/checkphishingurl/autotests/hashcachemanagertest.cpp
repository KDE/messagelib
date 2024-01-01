/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
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

HashCacheManagerTest::~HashCacheManagerTest() = default;

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

QTEST_GUILESS_MAIN(HashCacheManagerTest)

#include "moc_hashcachemanagertest.cpp"
