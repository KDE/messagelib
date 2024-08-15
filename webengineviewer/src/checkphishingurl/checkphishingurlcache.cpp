/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "checkphishingurlcache.h"
#include "checkphishingurlutil.h"
#include "webengineviewer_debug.h"
#include <KConfig>
#include <KConfigGroup>
#include <QMap>

using namespace WebEngineViewer;

struct UrlCacheInfo {
    UrlCacheInfo() = default;

    [[nodiscard]] bool isMalWare() const;
    [[nodiscard]] bool isValid() const;
    CheckPhishingUrlCache::UrlStatus status = CheckPhishingUrlCache::Unknown;
    uint verifyCacheAfterThisTime = 0;
};

bool UrlCacheInfo::isMalWare() const
{
    return status == CheckPhishingUrlCache::MalWare;
}

bool UrlCacheInfo::isValid() const
{
    return status != CheckPhishingUrlCache::Unknown;
}

class WebEngineViewer::CheckPhishingUrlCachePrivate
{
public:
    CheckPhishingUrlCachePrivate()
    {
        load();
    }

    [[nodiscard]] CheckPhishingUrlCache::UrlStatus urlStatus(const QUrl &url);
    void addCheckPhishingUrlResult(const QUrl &url, bool correctUrl, uint verifyCacheAfterThisTime);
    void clearCache();
    void load();
    void save();

private:
    QMap<QUrl, UrlCacheInfo> mCacheCheckedUrl;
};

void CheckPhishingUrlCachePrivate::clearCache()
{
    mCacheCheckedUrl.clear();
    save();
}

void CheckPhishingUrlCachePrivate::load()
{
    mCacheCheckedUrl.clear();
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("MalwareUrl"));
    const QList<QUrl> listMalware = grp.readEntry("Url", QList<QUrl>());
    const QList<double> listMalwareCachedTime = grp.readEntry("CachedTime", QList<double>());
    if (listMalware.count() != listMalwareCachedTime.count()) {
        qCWarning(WEBENGINEVIEWER_LOG) << "CheckPhishingUrlCachePrivate invalid number of data stored";
    } else {
        UrlCacheInfo info;
        const int numberOfMalware = listMalware.count();
        for (int i = 0; i < numberOfMalware; ++i) {
            info.status = WebEngineViewer::CheckPhishingUrlCache::MalWare;
            info.verifyCacheAfterThisTime = listMalwareCachedTime.at(i);
            if (WebEngineViewer::CheckPhishingUrlUtil::cachedValueStillValid(info.verifyCacheAfterThisTime)) {
                mCacheCheckedUrl.insert(listMalware.at(i), info);
            }
        }
    }
}

void CheckPhishingUrlCachePrivate::save()
{
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("MalwareUrl"));

    QList<QUrl> listMalware;
    QList<double> listMalwareCachedTime;

    QMap<QUrl, UrlCacheInfo>::const_iterator i = mCacheCheckedUrl.constBegin();
    const QMap<QUrl, UrlCacheInfo>::const_iterator end = mCacheCheckedUrl.constEnd();
    while (i != end) {
        const UrlCacheInfo info = i.value();
        if (info.isMalWare() && WebEngineViewer::CheckPhishingUrlUtil::cachedValueStillValid(info.verifyCacheAfterThisTime)) {
            listMalware.append(i.key());
            listMalwareCachedTime.append(info.verifyCacheAfterThisTime);
        }
        ++i;
    }
    grp.writeEntry("Url", listMalware);
    grp.writeEntry("CachedTime", listMalwareCachedTime);
    grp.sync();
}

CheckPhishingUrlCache::UrlStatus CheckPhishingUrlCachePrivate::urlStatus(const QUrl &url)
{
    const UrlCacheInfo info = mCacheCheckedUrl.value(url, UrlCacheInfo());
    if (info.isValid()) {
        if (info.verifyCacheAfterThisTime > 0) {
            if (CheckPhishingUrlUtil::cachedValueStillValid(info.verifyCacheAfterThisTime)) {
                return info.status;
            } else {
                return CheckPhishingUrlCache::Unknown;
            }
        } else {
            return info.status;
        }
    } else {
        return CheckPhishingUrlCache::Unknown;
    }
}

void CheckPhishingUrlCachePrivate::addCheckPhishingUrlResult(const QUrl &url, bool correctUrl, uint verifyCacheAfterThisTime)
{
    UrlCacheInfo info;
    info.status = correctUrl ? CheckPhishingUrlCache::UrlOk : CheckPhishingUrlCache::MalWare;
    info.verifyCacheAfterThisTime = correctUrl ? 0 : verifyCacheAfterThisTime;
    mCacheCheckedUrl.insert(url, info);
    if (info.status == CheckPhishingUrlCache::MalWare) {
        save();
    }
}

CheckPhishingUrlCache::CheckPhishingUrlCache(QObject *parent)
    : QObject(parent)
    , d(new CheckPhishingUrlCachePrivate)
{
}

CheckPhishingUrlCache::~CheckPhishingUrlCache() = default;

void CheckPhishingUrlCache::addCheckingUrlResult(const QUrl &url, bool correctUrl, uint verifyCacheAfterThisTime)
{
    d->addCheckPhishingUrlResult(url, correctUrl, verifyCacheAfterThisTime);
}

CheckPhishingUrlCache::UrlStatus CheckPhishingUrlCache::urlStatus(const QUrl &url)
{
    return d->urlStatus(url);
}

void CheckPhishingUrlCache::clearCache()
{
    d->clearCache();
}

CheckPhishingUrlCache *CheckPhishingUrlCache::self()
{
    static CheckPhishingUrlCache s_self;
    return &s_self;
}

#include "moc_checkphishingurlcache.cpp"
