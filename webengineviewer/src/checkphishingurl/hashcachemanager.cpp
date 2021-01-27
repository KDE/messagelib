/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "hashcachemanager.h"
#include "checkphishingurlutil.h"
#include "webengineviewer_debug.h"
#include <KConfig>
#include <KConfigGroup>

using namespace WebEngineViewer;

struct HashCacheInfo {
    HashCacheInfo()
    {
    }

    Q_REQUIRED_RESULT bool isValid() const;
    HashCacheManager::UrlStatus status = HashCacheManager::Unknown;
    uint verifyCacheAfterThisTime = 0;
};

bool HashCacheInfo::isValid() const
{
    return status != HashCacheManager::Unknown;
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
    Q_REQUIRED_RESULT HashCacheManager::UrlStatus hashStatus(const QByteArray &hash);

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
        switch (i.value().status) {
        case HashCacheManager::UrlOk:
            lstOk << i.key();
            lstOkDuration << i.value().verifyCacheAfterThisTime;
            break;
        case HashCacheManager::MalWare:
            lstMalware << i.key();
            lstMalwareDuration << i.value().verifyCacheAfterThisTime;
            break;
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
    case HashCacheManager::UrlOk:
        mHashList.insert(hash, info);
        break;
    case HashCacheManager::MalWare:
        mHashList.insert(hash, info);
        break;
    case HashCacheManager::Unknown:
        qCWarning(WEBENGINEVIEWER_LOG()) << "HashCacheManagerPrivate::addHashStatus unknown status detected!";
        return;
    }
    save();
}

HashCacheManager *HashCacheManager::self()
{
    static HashCacheManager s_self;
    return &s_self;
}

HashCacheManager::HashCacheManager(QObject *parent)
    : QObject(parent)
    , d(new HashCacheManagerPrivate)
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
