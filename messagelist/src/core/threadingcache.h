/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2016 Daniel Vrátil <dvratil@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "messageitem.h"
#include <QHash>

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

    Q_REQUIRED_RESULT bool isEnabled() const;
    void setEnabled(bool enabled);

    void load(const QString &id, const Aggregation *aggregation);
    void save();

    void addItemToCache(MessageItem *mi);

    void updateParent(MessageItem *mi, MessageItem *parent);

    MessageItem *parentForItem(MessageItem *mi, qint64 &parentId) const;

    void expireParent(MessageItem *item);

private:
    QHash<qint64 /* child */, qint64 /* parent */> mParentCache;
    QHash<qint64 /* ID */, MessageItem *> mItemCache;

    QString mCacheId;
    Aggregation::Grouping mGrouping = Aggregation::NoGrouping;
    Aggregation::Threading mThreading = Aggregation::NoThreading;
    Aggregation::ThreadLeader mThreadLeader = Aggregation::TopmostMessage;
    bool mEnabled = true;
};
} // namespace Core
} // namespace MessageList

