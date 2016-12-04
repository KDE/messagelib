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

#include "checkphishingurlcache.h"
#include <QMap>

using namespace WebEngineViewer;

Q_GLOBAL_STATIC(CheckPhishingUrlCache, s_checkPhishingUrlCache)

struct urlCacheInfo
{
    urlCacheInfo()
        : status(CheckPhishingUrlCache::Unknown),
          verifyCacheAfterThisTime(0)
    {

    }

    CheckPhishingUrlCache::UrlStatus status;
    uint verifyCacheAfterThisTime;
};

class WebEngineViewer::CheckPhishingUrlCachePrivate
{
public:
    CheckPhishingUrlCachePrivate()
    {

    }
    CheckPhishingUrlCache::UrlStatus urlStatus(const QUrl &url);
    void setCheckingUrlResult(const QUrl &url, bool correctUrl, uint verifyCacheAfterThisTime);
    void clearCache();
private:
    QMap<QUrl, CheckPhishingUrlCache::UrlStatus> mCacheCheckedUrl;
};

void CheckPhishingUrlCachePrivate::clearCache()
{
    mCacheCheckedUrl.clear();
}

CheckPhishingUrlCache::UrlStatus CheckPhishingUrlCachePrivate::urlStatus(const QUrl &url)
{
    return mCacheCheckedUrl.value(url, CheckPhishingUrlCache::Unknown);
}

void CheckPhishingUrlCachePrivate::setCheckingUrlResult(const QUrl &url, bool correctUrl, uint verifyCacheAfterThisTime)
{
    if (verifyCacheAfterThisTime > 0) {
        //TODO
    }
    mCacheCheckedUrl.insert(url, correctUrl ? CheckPhishingUrlCache::UrlOk : CheckPhishingUrlCache::MalWare);
}

CheckPhishingUrlCache::CheckPhishingUrlCache(QObject *parent)
    : QObject(parent),
      d(new CheckPhishingUrlCachePrivate)
{

}

CheckPhishingUrlCache::~CheckPhishingUrlCache()
{
    delete d;
}

void CheckPhishingUrlCache::setCheckingUrlResult(const QUrl &url, bool correctUrl, uint verifyCacheAfterThisTime)
{
    d->setCheckingUrlResult(url, correctUrl, verifyCacheAfterThisTime);
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
    return s_checkPhishingUrlCache;
}

