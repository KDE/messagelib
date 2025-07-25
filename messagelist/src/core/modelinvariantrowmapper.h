/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QHash>
using namespace Qt::Literals::StringLiterals;

#include <QList>
#include <QObject>

#include "core/modelinvariantindex.h"
#include <memory>
namespace MessageList
{
namespace Core
{
class ModelInvariantRowMapper;
class ModelInvariantRowMapperPrivate;

/**
 * This class is an optimizing helper for dealing with large flat QAbstractItemModel objects.
 *
 * The problem:
 *
 *   When you're an user of a _flat_ QAbstractItemModel you access its contents
 *   by the means of QModelIndex. The model index is basically a row index (for flat models).
 *   You usually fetch some data for a row and then store the row index in order
 *   to fetch more data later (think of a tree view that shows the "name" for an item
 *   but when clicked needs to open a window with the details associated to the item).
 *
 *   The problem is that your row indexes may become invalid when rows are added
 *   or removed from the model. For instance, if a row is added at position 10,
 *   then any cached index after position 10 must be updated in order to point to
 *   the same content. With very large models this can become a problem since
 *   the update must be somewhat "atomic" in order to preserve consistency.
 *   Atomic, then, means "unbreakable": you can't chunk it in smaller pieces.
 *   This also means that your application will simply freeze if you have a model
 *   with 100000 cached indexes and a row is added/removed at the beginning.
 *   Yet worse: your app will freeze EVERY time a row is added. This means
 *   that if you don't really optimize, or just add non contiguous rows, you
 *   must do the update multiple times...
 *
 * This class tries to solve this problem with a little overhead.
 * It basically gives you a ModelInvariantIndex for each "new" row you query.
 * Later the model may change by addition/removal of rows but with a ModelInvariantIndex
 * you will still be able to retrieve the content that the index was pointing
 * to at the time it was created.
 *
 * You don't need to implement any row update in your rowsInserted() / rowsRemoved()
 * handlers. Just call the modelRowsInserted() modelRowsRemoved() functions:
 * they will do everything for you in a substantially constant time.
 *
 * As the model structure changes the lookups will get a bit slower
 * since the row mapper must apply the changes sequentially to each invariant.
 * To avoid this, and to avoid storing the whole history of changes
 * the ModelInvariantRowMapper will perform a background update of your
 * ModelInvariantIndex objects. You don't need to care about this: it will happen automagically.
 *
 * The ModelInvariantIndex allocation and destruction is in fact left to you.
 * This is a GOOD approach because you're very likely to have some sort
 * of structures associated to the items you display. You may then simply
 * derive your objects from ModelInvariantIndex. This will save an additional
 * memory allocation for each one of your items (we are optimizing, aren't we ?)
 * and you will be able to dynamic_cast<> the ModelInvariantIndex pointers
 * directly to your structure pointers (which will likely save a large QHash<>...).
 * Even in the unlikely case in that you don't have a data structure for your items,
 * it's still an operator new() call that YOU make instead of this implementation.
 * It doesn't impact performance at all. You just have to remember to delete
 * your ModelInvariantIndex objects when you no longer need them.
 */
class ModelInvariantRowMapper : public QObject
{
    friend class ModelInvariantIndex;

    Q_OBJECT

public:
    explicit ModelInvariantRowMapper();
    ~ModelInvariantRowMapper() override;

    /**
     * Sets the maximum time we can spend inside a single lazy update step.
     * The larger this time, the more resources we consume and leave less to UI processing
     * but also larger the update throughput (that is, we update more items per second).
     * This is 50 msec by default.
     */
    void setLazyUpdateChunkInterval(int chunkInterval);

    /**
     * Sets the idle time between two lazy updates in milliseconds.
     * The larger this time, the less resources we consume and leave more to UI processing
     * but also smaller the update throughput (that is, we update less items per second).
     * This is 50 msec by default.
     */
    void setLazyUpdateIdleInterval(int idleInterval);

    /**
     * Maps a ModelInvariantIndex to the CURRENT associated row index in the model.
     * As long as the underlying model is consistent, the returned row index is guaranteed to
     * point to the content that this ModelInvariantIndex pointed at the time it was created.
     *
     * Returns the current associated row index in the model or -1 if the invariant
     * does not belong to this mapper (model) or is marked as invalid at all.
     */
    int modelInvariantIndexToModelIndexRow(ModelInvariantIndex *invariant);

    /**
     * Binds a ModelInvariantIndex structure to the specified CURRENT modelIndexRow.
     * Later you can use the ModelInvariantIndex to retrieve the model contents
     * that the parameter modelIndexRow refers to... even if the model changes.
     * Call this function only if you're sure that there is no such invariant yet,
     * otherwise take a look at modelIndexRowToModelInvariantIndex().
     *
     * This function ASSUMES that invariantToFill is a newly allocated ModelInvariantIndex.
     */
    void createModelInvariantIndex(int modelIndexRow, ModelInvariantIndex *invariantToFill);

    /**
     * Finds the existing ModelInvariantIndex that belongs to the specified CURRENT modelIndexRow.
     * Returns the ModelInvariantIndex found or 0 if such an invariant wasn't yet
     * created (by the means of createModelInvariantIndex()).
     */
    ModelInvariantIndex *modelIndexRowToModelInvariantIndex(int modelIndexRow);

    /**
     * This basically applies modelIndexRowToModelInvariantIndex() to a range of elements.
     * The returned pointer can be null if no existing ModelInvariantIndex object were
     * present in the range (this can happen if you don't request some of them). If the returned
     * value is not 0 then you're responsible of deleting it.
     */
    QList<ModelInvariantIndex *> *modelIndexRowRangeToModelInvariantIndexList(int startIndexRow, int count);

    /**
     * Call this function when rows are inserted to the underlying model
     * BEFORE scanning the model for the new items. You probably
     * want this function to be the first call in your rowsInserted() handler
     * or the last call in the rowsAboutToBeInserted() handler.
     */
    void modelRowsInserted(int modelIndexRowPosition, int count);

    /**
     * Call this function when rows are removed from the underlying model
     * AFTER accessing the removed rows for the last time. You probably
     * want this function to be the first call of your rowsRemoved() handler
     * or the last call in the rowsAboutToBeRemoved() handler.
     *
     * This function will invalidate any ModelInvariantIndex instances
     * that are affected by the change. It will also do you a favor by returning
     * the list of the invalidated ModelInvariantIndex objects since
     * you'll probably want to delete them. The returned pointer can be null
     * if no existing ModelInvariantIndex object were affected by the change
     * (this can happen if you don't request some of them). If the returned
     * value is not 0 then you're responsible of deleting it.
     */
    QList<ModelInvariantIndex *> *modelRowsRemoved(int modelIndexRowPosition, int count);

    /**
     * Call this function from your handlers of reset() and layoutChanged()
     * AFTER you ve last accessed the model underlying data.
     * You probably want this function to be the first call of your
     * reset() or layoutChanged() handlers.
     *
     * This function assumes that all the ModelInvariantIndex
     * are being invalidated and need to be required.
     */
    void modelReset();

private:
    std::unique_ptr<ModelInvariantRowMapperPrivate> const d;
};
} // namespace Core
} // namespace MessageList
