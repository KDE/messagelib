/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2016 Daniel Vrátil <dvratil@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#ifndef THREADINGCACHE_H
#define THREADINGCACHE_H

#include <QHash>
#include "messageitem.h"

namespace MessageList {
namespace Core {
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

    void addItemToCache(MessageItem *mi);

    void updateParent(MessageItem *mi, MessageItem *parent);

    MessageItem *parentForItem(MessageItem *mi, qint64 &parentId) const;

    void expireParent(MessageItem *item);

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
