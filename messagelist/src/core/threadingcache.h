/******************************************************************************
 *
 *  Copyright 2016 Daniel Vrátil <dvratil@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *******************************************************************************/

#ifndef THREADINGCACHE_H
#define THREADINGCACHE_H

#include <QHash>
#include "messageitem.h"

namespace MessageList
{

namespace Core
{

class Aggregation;

class ThreadingCache
{
public:
    ThreadingCache();
    ~ThreadingCache();

    bool isEnabled() const;
    void setEnabled(bool enabled);

    void load(const QString &id, const Aggregation *aggregation);
    void save();

    void addItemToCache(MessageItem *mi)
    {
        if (mEnabled) {
            mItemCache.insert(mi->itemId(), mi);
        }
    }

    void updateParent(MessageItem *mi, MessageItem *parent)
    {
        if (mEnabled) {
            mParentCache.insert(mi->itemId(), parent ? parent->itemId() : 0);
        }
    }

    MessageItem *parentForItem(MessageItem *mi, qint64 &parentId) const
    {
        if (mEnabled) {
            parentId = mParentCache.value(mi->itemId(), -1);
            if (parentId > -1) {
                return mItemCache.value(parentId, Q_NULLPTR);
            } else {
                return Q_NULLPTR;
            }
        } else {
            return Q_NULLPTR;
        }
    }

    void expireParent(MessageItem *item)
    {
        if (mEnabled) {
            mParentCache.remove(item->itemId());
        }
    }

private:
    QString cacheFilePath(const QString &id) const;

    QHash<qint64 /* child */, qint64 /* parent */> mParentCache;
    QHash<qint64 /* ID */, MessageItem *> mItemCache;

    QString mCacheId;
    Aggregation::Grouping mGrouping;
    Aggregation::Threading mThreading;
    Aggregation::ThreadLeader mThreadLeader;
    bool mEnabled;
};

} // namespace Core
} // namespace MessageList

#endif
