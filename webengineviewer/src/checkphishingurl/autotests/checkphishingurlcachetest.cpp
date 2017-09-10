/*
   Copyright (C) 2016-2017 Laurent Montel <montel@kde.org>

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

#include "checkphishingurlcachetest.h"
#include "../checkphishingurlcache.h"
#include "../checkphishingurlutil.h"
#include <QTest>
#include <QStandardPaths>
#include <KConfig>
#include <kconfiggroup.h>

CheckPhishingUrlCacheTest::CheckPhishingUrlCacheTest(QObject *parent)
    : QObject(parent)
{
    QStandardPaths::setTestModeEnabled(true);
}

CheckPhishingUrlCacheTest::~CheckPhishingUrlCacheTest()
{
}

void CheckPhishingUrlCacheTest::shouldNotBeAMalware()
{
    WebEngineViewer::CheckPhishingUrlCache cache;
    QCOMPARE(cache.urlStatus(QUrl(QStringLiteral("http://www.kde.org"))), WebEngineViewer::CheckPhishingUrlCache::Unknown);
}

void CheckPhishingUrlCacheTest::shouldAddValue_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<bool>("validurl");
    QTest::addColumn<uint>("seconds");
    QTest::addColumn<WebEngineViewer::CheckPhishingUrlCache::UrlStatus>("status");
    uint currentValue = QDateTime::currentDateTimeUtc().toTime_t();
    QTest::newRow("valid") << QUrl(QStringLiteral("http://www.kde.org")) << true << currentValue << WebEngineViewer::CheckPhishingUrlCache::UrlOk;
    QTest::newRow("malware1validcache") << QUrl(QStringLiteral("http://www.kde.org")) << false << (currentValue + 2000) << WebEngineViewer::CheckPhishingUrlCache::MalWare;
    QTest::newRow("malware1invalidcache") << QUrl(QStringLiteral("http://www.kde.org")) << false << (currentValue - 2000) << WebEngineViewer::CheckPhishingUrlCache::Unknown;
}

void CheckPhishingUrlCacheTest::shouldAddValue()
{
    QFETCH(QUrl, url);
    QFETCH(bool, validurl);
    QFETCH(uint, seconds);
    QFETCH(WebEngineViewer::CheckPhishingUrlCache::UrlStatus, status);

    WebEngineViewer::CheckPhishingUrlCache cache;
    cache.addCheckingUrlResult(url, validurl, seconds);
    QCOMPARE(cache.urlStatus(url), status);
    cache.clearCache();
}

void CheckPhishingUrlCacheTest::shouldStoreValues()
{
    WebEngineViewer::CheckPhishingUrlCache cache;
    QUrl url = QUrl(QStringLiteral("http://www.kde.org"));

    uint currentValue = QDateTime::currentDateTimeUtc().toTime_t();
    cache.addCheckingUrlResult(url, false, currentValue + 2000);

    //Add malware
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("MalwareUrl"));
    QList<QUrl> listMalware = grp.readEntry("Url", QList<QUrl>());
    QList<double> listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());

    QCOMPARE(listMalware.count(), 1);
    QCOMPARE(listMalwareCachedTime.count(), 1);

    //Clear cache
    cache.clearCache();
    KConfig phishingurlKConfig2(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    grp = phishingurlKConfig2.group(QStringLiteral("MalwareUrl"));
    listMalware = grp.readEntry("Url", QList<QUrl>());
    listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());

    QCOMPARE(listMalware.count(), 0);
    QCOMPARE(listMalwareCachedTime.count(), 0);

    //Add correct url
    cache.clearCache();

    cache.addCheckingUrlResult(url, true, 0);

    KConfig phishingurlKConfig3(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    grp = phishingurlKConfig3.group(QStringLiteral("MalwareUrl"));
    listMalware = grp.readEntry("Url", QList<QUrl>());
    listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());

    QCOMPARE(listMalware.count(), 0);
    QCOMPARE(listMalwareCachedTime.count(), 0);

    cache.clearCache();

    //Load another instance => data were saved
    cache.addCheckingUrlResult(url, false, currentValue + 2000);

    WebEngineViewer::CheckPhishingUrlCache cache2;
    KConfig phishingurlKConfig4(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    grp = phishingurlKConfig4.group(QStringLiteral("MalwareUrl"));
    listMalware = grp.readEntry("Url", QList<QUrl>());
    listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());

    QCOMPARE(listMalware.count(), 1);
    QCOMPARE(listMalwareCachedTime.count(), 1);

    //Need to clear to avoid problem when we relaunch this autotest
    cache2.clearCache();
    cache.clearCache();
}

QTEST_MAIN(CheckPhishingUrlCacheTest)
