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

#include "checkphishingurlcachetest.h"
#include "../checkphishingurlcache.h"
#include <QTest>
#include <QStandardPaths>

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
    QTest::addColumn<double>("seconds");
    QTest::addColumn<WebEngineViewer::CheckPhishingUrlCache::UrlStatus>("status");
    double currentValue = QDateTime::currentDateTime().toTime_t();
    QTest::newRow("valid") << QUrl(QStringLiteral("http://www.kde.org")) << true << currentValue << WebEngineViewer::CheckPhishingUrlCache::UrlOk;
    QTest::newRow("malware1validcache") << QUrl(QStringLiteral("http://www.kde.org")) << false << (currentValue + 2000) << WebEngineViewer::CheckPhishingUrlCache::MalWare;
    QTest::newRow("malware1invalidcache") << QUrl(QStringLiteral("http://www.kde.org")) << false << (currentValue - 2000) << WebEngineViewer::CheckPhishingUrlCache::Unknown;
}

void CheckPhishingUrlCacheTest::shouldAddValue()
{
    QFETCH(QUrl, url);
    QFETCH(bool, validurl);
    QFETCH(double, seconds);
    QFETCH(WebEngineViewer::CheckPhishingUrlCache::UrlStatus, status);

    WebEngineViewer::CheckPhishingUrlCache cache;
    cache.setCheckingUrlResult(url, validurl, seconds);
    QCOMPARE(cache.urlStatus(url), status);
    cache.clearCache();
}

QTEST_MAIN(CheckPhishingUrlCacheTest)
