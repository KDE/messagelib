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

#include "threadingcache.h"
#include "aggregation.h"
#include "messagelist_debug.h"

#include <QStandardPaths>
#include <QFile>
#include <QDir>

using namespace MessageList::Core;

namespace {
struct CacheHeader {
    int version;
    Aggregation::Grouping grouping;
    Aggregation::Threading threading;
    Aggregation::ThreadLeader threadLeader;
    int cacheSize;
};

QDataStream &operator<<(QDataStream &stream, const CacheHeader &header)
{
    return stream << header.version
                  << (qint8)header.grouping
                  << (qint8)header.threading
                  << (qint8)header.threadLeader
                  << header.cacheSize;
}

QDataStream &operator>>(QDataStream &stream, CacheHeader &header)
{
    return stream >> header.version
           >> (qint8 &)header.grouping
           >> (qint8 &)header.threading
           >> (qint8 &)header.threadLeader
           >> header.cacheSize;
}
}

ThreadingCache::ThreadingCache()
    : mGrouping(Aggregation::NoGrouping)
    , mThreading(Aggregation::NoThreading)
    , mThreadLeader(Aggregation::TopmostMessage)
    , mEnabled(true)
{
}

ThreadingCache::~ThreadingCache()
{
    if (mEnabled && !mCacheId.isEmpty()) {
        save();
    }
}

bool ThreadingCache::isEnabled() const
{
    return mEnabled;
}

void ThreadingCache::setEnabled(bool enabled)
{
    mEnabled = enabled;
}

void ThreadingCache::load(const QString &id, const Aggregation *aggregation)
{
    mParentCache.clear();
    mItemCache.clear();

    mCacheId = id;
    mThreading = aggregation->threading();
    mThreadLeader = aggregation->threadLeader();
    mGrouping = aggregation->grouping();
    mEnabled = true;

    const QString cacheFileName = QStandardPaths::locate(QStandardPaths::CacheLocation,
                                                         QStringLiteral("messagelist/threading/%1").arg(id),
                                                         QStandardPaths::LocateFile);
    if (cacheFileName.isEmpty()) {
        qCDebug(MESSAGELIST_LOG) << "No threading cache file for collection" << id;
        return;
    }
    qCDebug(MESSAGELIST_LOG) << "Loading threading cache file" << cacheFileName;

    QFile cacheFile(cacheFileName);
    if (!cacheFile.open(QIODevice::ReadOnly)) {
        qCWarning(MESSAGELIST_LOG) << "Failed to open cache file" << cacheFileName << ":" << cacheFile.errorString();
        return;
    }

    QDataStream stream(&cacheFile);
    CacheHeader cacheHeader = {};
    stream >> cacheHeader;

    if (cacheHeader.version != 1) {
        // Unknown version
        qCDebug(MESSAGELIST_LOG) << "\tCache file unusable, unknown version";
        cacheFile.close();
        cacheFile.remove();
        return;
    }

    if (cacheHeader.grouping != mGrouping
        || cacheHeader.threading != mThreading
        || cacheHeader.threadLeader != mThreadLeader) {
        // The cache is valid, but for a different grouping/threading configuration.
        qCDebug(MESSAGELIST_LOG) << "\tCache file unusable, threading configuration mismatch";
        cacheFile.close();
        cacheFile.remove();
        return;
    }

    mItemCache.reserve(cacheHeader.cacheSize);
    mParentCache.reserve(cacheHeader.cacheSize);

    for (int i = 0; i < cacheHeader.cacheSize; ++i) {
        qint64 child, parent;
        stream >> child >> parent;
        if (stream.status() != QDataStream::Ok) {
            // Suspect corrupted cache
            qCDebug(MESSAGELIST_LOG) << "\tCache file unusable, data truncated";
            cacheFile.close();
            cacheFile.remove();
            mParentCache.clear();
            return;
        }

        mParentCache.insert(child, parent);
    }

    qCDebug(MESSAGELIST_LOG) << "Loaded" << cacheHeader.cacheSize << "entries from threading cache";
}

void ThreadingCache::save()
{
    if (mCacheId.isEmpty()) {
        return;
    }
    const QDir cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (!cacheDir.exists(QStringLiteral("messagelist/threading"))) {
        if (!cacheDir.mkpath(QStringLiteral("messagelist/threading"))) {
            qCWarning(MESSAGELIST_LOG) << "Failed to create cache directory.";
            return;
        }
    }

    QFile cacheFile(cacheDir.filePath(QStringLiteral("messagelist/threading/%1").arg(mCacheId)));
    if (!cacheFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qCWarning(MESSAGELIST_LOG) << "Failed to create cache file:" << cacheFile.errorString() << " mCacheId " << mCacheId;
        return;
    }

    qCDebug(MESSAGELIST_LOG) << "Saving threading cache to" << cacheFile.fileName();

    QDataStream stream(&cacheFile);
    CacheHeader cacheHeader{
        1,                      // version
        mGrouping,
        mThreading,
        mThreadLeader,
        mParentCache.size()
    };
    stream << cacheHeader;
    cacheFile.flush();

    for (auto iter = mParentCache.cbegin(), end = mParentCache.cend(); iter != end; ++iter) {
        stream << iter.key() << iter.value();
    }
    qCDebug(MESSAGELIST_LOG) << "Saved" << mParentCache.count() << "cache entries";
}

void ThreadingCache::addItemToCache(MessageItem *mi)
{
    if (mEnabled) {
        mItemCache.insert(mi->itemId(), mi);
    }
}

void ThreadingCache::updateParent(MessageItem *mi, MessageItem *parent)
{
    if (mEnabled) {
        mParentCache.insert(mi->itemId(), parent ? parent->itemId() : 0);
    }
}

MessageItem *ThreadingCache::parentForItem(MessageItem *mi, qint64 &parentId) const
{
    if (mEnabled) {
        parentId = mParentCache.value(mi->itemId(), -1);
        if (parentId > -1) {
            return mItemCache.value(parentId, nullptr);
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

void ThreadingCache::expireParent(MessageItem *item)
{
    if (mEnabled) {
        mParentCache.remove(item->itemId());
        mItemCache.remove(item->itemId());
    }
}
