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

#ifndef CHECKPHISHINGURLCACHE_H
#define CHECKPHISHINGURLCACHE_H

#include "webengineviewer_export.h"
#include <QObject>
#include <QUrl>

namespace WebEngineViewer
{
class CheckPhishingUrlCachePrivate;
class WEBENGINEVIEWER_EXPORT CheckPhishingUrlCache: public QObject
{
    Q_OBJECT
public:
    static CheckPhishingUrlCache *self();

    enum UrlStatus {
        UrlOk = 0,
        MalWare = 1,
        Unknown = 2
    };

    explicit CheckPhishingUrlCache(QObject *parent = Q_NULLPTR);
    ~CheckPhishingUrlCache();

    /**
     * @brief setCheckingUrlResult cache url. If @p correctUrl is true we store as UrlOk otherwise MalWare
     * @param url
     * @param correctUrl
     */
    void setCheckingUrlResult(const QUrl &url, bool correctUrl);
    /**
     * @brief urlStatus Return the status of cached Url. When we didn't stored it it returns Unknown
     * @param url
     * @return the status of url
     */
    CheckPhishingUrlCache::UrlStatus urlStatus(const QUrl &url);

    void clearCache();
private:
    CheckPhishingUrlCachePrivate *const d;
};
}

#endif // CHECKPHISHINGURLCACHE_H
