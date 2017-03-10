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

#ifndef HASHCACHEMANAGER_H
#define HASHCACHEMANAGER_H

#include <QObject>
#include "webengineviewer_export.h"

namespace WebEngineViewer
{
class HashCacheManagerPrivate;
//https://developers.google.com/safe-browsing/v4/caching
class WEBENGINEVIEWER_EXPORT HashCacheManager : public QObject
{
    Q_OBJECT
public:
    enum UrlStatus {
        UrlOk = 0,
        MalWare = 1,
        Unknown = 2
    };
    explicit HashCacheManager(QObject *parent = nullptr);
    ~HashCacheManager();

    static HashCacheManager *self();

    void clearCache();

    void addHashStatus(const QByteArray &hash, HashCacheManager::UrlStatus status, uint cacheDuration);

    HashCacheManager::UrlStatus hashStatus(const QByteArray &hash);

private:
    HashCacheManagerPrivate *const d;
};
}

Q_DECLARE_METATYPE(WebEngineViewer::HashCacheManager::UrlStatus)
#endif // HASHCACHEMANAGER_H
