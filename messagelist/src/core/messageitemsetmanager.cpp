/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/messageitemsetmanager.h"
#include "core/messageitem.h"

using namespace MessageList::Core;

MessageItemSetManager::MessageItemSetManager()
{
    mSets = new QHash<MessageItemSetReference, QHash<MessageItem *, MessageItem *> *>();
}

MessageItemSetManager::~MessageItemSetManager()
{
    clearAllSets();
    delete mSets;
}

void MessageItemSetManager::clearAllSets()
{
    for (auto it = mSets->cbegin(), end = mSets->cend(); it != end; ++it) {
        delete (*it);
    }
    mSets->clear();
}

int MessageItemSetManager::setCount() const
{
    return mSets->count();
}

void MessageItemSetManager::removeSet(MessageItemSetReference ref)
{
    auto set = mSets->value(static_cast<ulong>(ref), nullptr);
    if (!set) {
        return;
    }
    mSets->remove(static_cast<ulong>(ref));
    delete set;
}

QList<MessageItem *> MessageItemSetManager::messageItems(MessageItemSetReference ref)
{
    auto set = mSets->value(static_cast<ulong>(ref), nullptr);
    if (!set) {
        return {};
    }

    return set->values();
}

void MessageItemSetManager::removeMessageItemFromAllSets(MessageItem *mi)
{
    QVector<MessageItemSetReference> setsToBeKilled;

    for (auto it = mSets->cbegin(), end = mSets->cend(); it != end; ++it) {
        (*it)->remove(mi);
        if ((*it)->isEmpty()) {
            setsToBeKilled.append(it.key());
        }
    }

    for (const auto set : setsToBeKilled) {
        removeSet(set);
    }
}

MessageItemSetReference MessageItemSetManager::createSet()
{
    static unsigned long int uNextId = 0;
    uNextId++;

    auto set = mSets->value(uNextId, nullptr);

    // Here we assume that having 2^32 sets is impossible
    // (that would be _at_least_ 2^32 * 8 bytes of allocated memory which is
    // quite a lot...)

    while (set) {
        uNextId++;
        set = mSets->value(uNextId, nullptr);
    }

    set = new QHash<MessageItem *, MessageItem *>();
    mSets->insert(uNextId, set);

    return static_cast<MessageItemSetReference>(uNextId);
}

bool MessageItemSetManager::addMessageItem(MessageItemSetReference ref, MessageItem *mi)
{
    auto set = mSets->value(static_cast<ulong>(ref), nullptr);
    if (!set) {
        return false;
    }

    set->insert(mi, mi);

    return true;
}
