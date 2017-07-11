/******************************************************************************
 *
 *  Copyright 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
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
        delete(*it);
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

QList< MessageItem * > MessageItemSetManager::messageItems(MessageItemSetReference ref)
{
    auto *set = mSets->value(static_cast<ulong>(ref), nullptr);
    if (!set) {
        return {};
    }

    return set->values();
}

void MessageItemSetManager::removeMessageItemFromAllSets(MessageItem *mi)
{
    QList< MessageItemSetReference > setsToBeKilled;

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

    return static_cast< MessageItemSetReference >(uNextId);
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

