/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QHash>
#include <QList>

namespace MessageList
{
namespace Core
{
class MessageItem;

using MessageItemSetReference = long;

/**
 * This class manages sets of messageitem references.
 * It can be used to create a set, add some messages to it
 * and get a reference that later can be used to retrieve
 * the stored messages.
 *
 * It's used by Model to keep track of jobs requested
 * from outside that operate on sets of MessageItem instances.
 * Model takes care of removing the deleted MessageItem objects
 * from the sets in order to avoid invalid references.
 */
class MessageItemSetManager
{
public:
    MessageItemSetManager();
    ~MessageItemSetManager();

private:
    QHash<MessageItemSetReference, QHash<MessageItem *, MessageItem *> *> *mSets;

public:
    void clearAllSets();
    Q_REQUIRED_RESULT int setCount() const;
    void removeSet(MessageItemSetReference ref);
    void removeMessageItemFromAllSets(MessageItem *mi);
    Q_REQUIRED_RESULT QList<MessageItem *> messageItems(MessageItemSetReference ref);
    Q_REQUIRED_RESULT MessageItemSetReference createSet();
    Q_REQUIRED_RESULT bool addMessageItem(MessageItemSetReference ref, MessageItem *mi);
};
} // namespace Core
} // namespace MessageList

