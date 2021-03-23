/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "core/modelinvariantrowmapper.h"

#include <QTime>
#include <QTimer>

namespace MessageList
{
namespace Core
{
class ModelInvariantRowMapperPrivate
{
public:
    explicit ModelInvariantRowMapperPrivate(ModelInvariantRowMapper *owner)
        : q(owner)
    {
    }

    /**
     * Internal. Don't look a this :)
     */
    void updateModelInvariantIndex(int modelIndexRow, ModelInvariantIndex *invariantToFill);

    /**
     * Internal. Don't look a this :)
     */
    ModelInvariantIndex *modelIndexRowToModelInvariantIndexInternal(int modelIndexRow, bool updateIfNeeded);

    /**
     * Internal: Removes the first RowShift from the list.
     */
    void killFirstRowShift();

    /**
     * This is called from the ModelInvariantIndex destructor.
     * You don't need to care.
     */
    void indexDead(ModelInvariantIndex *index);

    /**
     * Internal: Performs a lazy update step.
     */
    void slotPerformLazyUpdate();

    ModelInvariantRowMapper *const q;

    QList<RowShift *> *mRowShiftList = nullptr; ///< The ordered list of RowShifts, most recent at the end
    QHash<int, ModelInvariantIndex *> *mCurrentInvariantHash; ///< The up-to-date invariants
    uint mCurrentShiftSerial; ///< Current model change serial: FIXME: it explodes at 2^32 :D
    uint mRemovedShiftCount; ///< The number of shifts that we have completely processed
    int mLazyUpdateChunkInterval; ///< Msecs: how much time we spend inside a lazy update chunk
    int mLazyUpdateIdleInterval; ///< Msecs: how much time we idle between lazy update chunks
    QTimer *mUpdateTimer = nullptr; ///< Background lazy update timer
};
} // Core
} // MessageList

