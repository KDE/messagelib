/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkphishingurlcachetest.h"
#include "../checkphishingurlcache.h"
#include "../checkphishingurlutil.h"
#include <KConfig>
#include <KConfigGroup>
#include <QStandardPaths>
#include <QTest>

CheckPhishingUrlCacheTest::CheckPhishingUrlCacheTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

CheckPhishingUrlCacheTest::~CheckPhishingUrlCacheTest() = default;

void CheckPhishingUrlCacheTest::shouldNotBeAMalware()
{
    auto cache = WebEngineViewer::CheckPhishingUrlCache::self();
    QCOMPARE(cache->urlStatus(QUrl(QStringLiteral("http://www.kde.org"))), WebEngineViewer::CheckPhishingUrlCache::Unknown);
}

void CheckPhishingUrlCacheTest::shouldAddValue_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<bool>("validurl");
    QTest::addColumn<uint>("seconds");
    QTest::addColumn<WebEngineViewer::CheckPhishingUrlCache::UrlStatus>("status");
    uint currentValue = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
    QTest::newRow("valid") << QUrl(QStringLiteral("http://www.kde.org")) << true << currentValue << WebEngineViewer::CheckPhishingUrlCache::UrlOk;
    QTest::newRow("malware1validcache") << QUrl(QStringLiteral("http://www.kde.org")) << false << (currentValue + 2000)
                                        << WebEngineViewer::CheckPhishingUrlCache::MalWare;
    QTest::newRow("malware1invalidcache") << QUrl(QStringLiteral("http://www.kde.org")) << false << (currentValue - 2000)
                                          << WebEngineViewer::CheckPhishingUrlCache::Unknown;
}

void CheckPhishingUrlCacheTest::shouldAddValue()
{
    QFETCH(QUrl, url);
    QFETCH(bool, validurl);
    QFETCH(uint, seconds);
    QFETCH(WebEngineViewer::CheckPhishingUrlCache::UrlStatus, status);

    auto cache = WebEngineViewer::CheckPhishingUrlCache::self();
    cache->addCheckingUrlResult(url, validurl, seconds);
    QCOMPARE(cache->urlStatus(url), status);
    cache->clearCache();
}

void CheckPhishingUrlCacheTest::shouldStoreValues()
{
    auto cache = WebEngineViewer::CheckPhishingUrlCache::self();
    QUrl url = QUrl(QStringLiteral("http://www.kde.org"));

    uint currentValue = QDateTime::currentDateTimeUtc().toSecsSinceEpoch();
    cache->addCheckingUrlResult(url, false, currentValue + 2000);

    // Add malware
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("MalwareUrl"));
    QList<QUrl> listMalware = grp.readEntry("Url", QList<QUrl>());
    QList<double> listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());

    QCOMPARE(listMalware.count(), 1);
    QCOMPARE(listMalwareCachedTime.count(), 1);

    // Clear cache
    cache->clearCache();
    KConfig phishingurlKConfig2(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    grp = phishingurlKConfig2.group(QStringLiteral("MalwareUrl"));
    listMalware = grp.readEntry("Url", QList<QUrl>());
    listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());

    QCOMPARE(listMalware.count(), 0);
    QCOMPARE(listMalwareCachedTime.count(), 0);

    // Add correct url
    cache->clearCache();

    cache->addCheckingUrlResult(url, true, 0);

    KConfig phishingurlKConfig3(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    grp = phishingurlKConfig3.group(QStringLiteral("MalwareUrl"));
    listMalware = grp.readEntry("Url", QList<QUrl>());
    listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());

    QCOMPARE(listMalware.count(), 0);
    QCOMPARE(listMalwareCachedTime.count(), 0);

    cache->clearCache();

    // Load another instance => data were saved
    cache->addCheckingUrlResult(url, false, currentValue + 2000);

    WebEngineViewer::CheckPhishingUrlCache cache2;
    KConfig phishingurlKConfig4(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    grp = phishingurlKConfig4.group(QStringLiteral("MalwareUrl"));
    listMalware = grp.readEntry("Url", QList<QUrl>());
    listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());

    QCOMPARE(listMalware.count(), 1);
    QCOMPARE(listMalwareCachedTime.count(), 1);

    // Need to clear to avoid problem when we relaunch this autotest
    cache2.clearCache();
    cache->clearCache();
}

QTEST_MAIN(CheckPhishingUrlCacheTest)

#include "moc_checkphishingurlcachetest.cpp"
