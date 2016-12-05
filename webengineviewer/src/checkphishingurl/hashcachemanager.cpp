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

struct HashCacheInfo
{
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
private:
    QList<HashCacheInfo> mMalwareList;
    QList<HashCacheInfo> mOkList;
};

void HashCacheManagerPrivate::clearCache()
{
    mMalwareList.clear();
    mOkList.clear();
    save();
}

void HashCacheManagerPrivate::save()
{
    //TODO
}

void HashCacheManagerPrivate::load()
{
    //TODO
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
