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

#include "hashcachemanager.h"
#include "webengineviewer_debug.h"
#include "checkphishingurlutil.h"
#include <KConfig>
#include <KConfigGroup>

using namespace WebEngineViewer;

Q_GLOBAL_STATIC(HashCacheManager, s_hashCacheManager)

struct HashCacheInfo {
    HashCacheInfo()
        : status(HashCacheManager::Unknown),
          verifyCacheAfterThisTime(0)
    {

    }
    bool isValid() const;
    HashCacheManager::UrlStatus status;
    uint verifyCacheAfterThisTime;
};

bool HashCacheInfo::isValid() const
{
    return (status != HashCacheManager::Unknown);
}

class WebEngineViewer::HashCacheManagerPrivate
{
public:
    HashCacheManagerPrivate()
    {
        load();
    }
    void clearCache();
    void save();
    void load();
    void addHashStatus(const QByteArray &hash, HashCacheManager::UrlStatus status, uint cacheDuration);
    HashCacheManager::UrlStatus hashStatus(const QByteArray &hash);
private:
    void clear();

    QMap<QByteArray, HashCacheInfo> mHashList;
};

void HashCacheManagerPrivate::clear()
{
    mHashList.clear();
}

void HashCacheManagerPrivate::clearCache()
{
    clear();
    save();
}

void HashCacheManagerPrivate::save()
{
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("Hash"));

    QList<QByteArray> lstMalware;
    QList<double> lstMalwareDuration;

    QList<QByteArray> lstOk;
    QList<double> lstOkDuration;

    QMapIterator<QByteArray, HashCacheInfo> i(mHashList);
    while (i.hasNext()) {
        i.next();
        switch(i.value().status) {
        case HashCacheManager::UrlOk: {
            lstOk << i.key();
            lstOkDuration << i.value().verifyCacheAfterThisTime;
            break;
        }
        case HashCacheManager::MalWare: {
            lstMalware << i.key();
            lstMalwareDuration << i.value().verifyCacheAfterThisTime;
            break;
        }
        case HashCacheManager::Unknown:
            break;
        }
    }
    grp.writeEntry("malware", lstMalware);
    grp.writeEntry("malwareCacheDuration", lstMalwareDuration);

    grp.writeEntry("safe", lstOk);
    grp.writeEntry("safeCacheDuration", lstOkDuration);
    grp.sync();
}

void HashCacheManagerPrivate::load()
{
    clear();
    KConfig phishingurlKConfig(WebEngineViewer::CheckPhishingUrlUtil::configFileName());
    KConfigGroup grp = phishingurlKConfig.group(QStringLiteral("Hash"));
    QList<QByteArray> lstMalware = grp.readEntry("malware", QList<QByteArray>());
    QList<double> lstMalwareDuration = grp.readEntry("malwareCacheDuration", QList<double>());

    QList<QByteArray> lstOk = grp.readEntry("safe", QList<QByteArray>());
    QList<double> lstOkDuration = grp.readEntry("safeCacheDuration", QList<double>());
    if (lstMalware.count() != lstMalwareDuration.count()) {
        qCWarning(WEBENGINEVIEWER_LOG) << "unsafe url: HashCacheManagerPrivate invalid number of data stored";
        return;
    }
    if (lstOk.count() != lstOkDuration.count()) {
        qCWarning(WEBENGINEVIEWER_LOG) << "safe url: HashCacheManagerPrivate invalid number of data stored";
        return;
    }

    const int nb(lstOk.count());
    for (int i = 0; i < nb; ++i) {
        HashCacheInfo info;
        info.status = HashCacheManager::UrlOk;
        info.verifyCacheAfterThisTime = lstOkDuration.at(i);
        if (info.isValid()) {
            mHashList.insert(lstOk.at(i), info);
        }
    }

    const int nb2(lstMalware.count());
    for (int i = 0; i < nb2; ++i) {
        HashCacheInfo info;
        info.status = HashCacheManager::MalWare;
        info.verifyCacheAfterThisTime = lstMalwareDuration.at(i);
        if (info.isValid()) {
            mHashList.insert(lstMalware.at(i), info);
        }
    }
}

HashCacheManager::UrlStatus HashCacheManagerPrivate::hashStatus(const QByteArray &hash)
{
    const HashCacheInfo info = mHashList.value(hash, HashCacheInfo());
    if (info.isValid()) {
        if (CheckPhishingUrlUtil::cachedValueStillValid(info.verifyCacheAfterThisTime)) {
            return info.status;
        } else {
            return HashCacheManager::Unknown;
        }
    } else {
        return HashCacheManager::Unknown;
    }
}

void HashCacheManagerPrivate::addHashStatus(const QByteArray &hash, HashCacheManager::UrlStatus status, uint cacheDuration)
{
    HashCacheInfo info;
    info.status = status;
    info.verifyCacheAfterThisTime = cacheDuration;
    switch (status) {
    case HashCacheManager::UrlOk: {
        mHashList.insert(hash, info);
        break;
    }
    case HashCacheManager::MalWare: {
        mHashList.insert(hash, info);
        break;
    }
    case HashCacheManager::Unknown:
        qCWarning(WEBENGINEVIEWER_LOG()) << "HashCacheManagerPrivate::addHashStatus unknow status detected!";
        return;
    }
    save();
}

HashCacheManager *HashCacheManager::self()
{
    return s_hashCacheManager;
}

HashCacheManager::HashCacheManager(QObject *parent)
    : QObject(parent),
      d(new HashCacheManagerPrivate)
{

}

HashCacheManager::~HashCacheManager()
{
    delete d;
}

void HashCacheManager::clearCache()
{
    d->clearCache();
}

void HashCacheManager::addHashStatus(const QByteArray &hash, HashCacheManager::UrlStatus status, uint cacheDuration)
{
    d->addHashStatus(hash, status, cacheDuration);
}

HashCacheManager::UrlStatus HashCacheManager::hashStatus(const QByteArray &hash)
{
    return d->hashStatus(hash);
}
