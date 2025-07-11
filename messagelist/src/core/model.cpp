/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

//
// This class is a rather huge monster. It's something that resembles a QAbstractItemModel
// (because it has to provide the interface for a QTreeView) but isn't entirely one
// (for optimization reasons). It basically manages a tree of items of two types:
// GroupHeaderItem and MessageItem. Be sure to read the docs for ViewItemJob.
//
// A huge credit here goes to Till Adam which seems to have written most
// (if not all) of the original KMail threading code. The KMHeaders implementation,
// the documentation and his clever ideas were my starting points and essential tools.
// This is why I'm adding his copyright entry (copied from headeritem.cpp) here even if
// he didn't write a byte in this file until now :)
//
//                                       Szymon Tomasz Stefanek, 03 Aug 2008 04:50 (am)
//
// This class contains ideas from:
//
//   kmheaders.cpp / kmheaders.h, headeritem.cpp / headeritem.h
//   Copyright: (c) 2004 Till Adam < adam at kde dot org >
//
#include "core/model.h"
using namespace Qt::Literals::StringLiterals;

#include "core/filter.h"
#include "core/groupheaderitem.h"
#include "core/item_p.h"
#include "core/manager.h"
#include "core/messageitem.h"
#include "core/messageitemsetmanager.h"
#include "core/model_p.h"
#include "core/modelinvariantrowmapper.h"
#include "core/storagemodelbase.h"
#include "core/theme.h"
#include "core/view.h"
#include "messagelist_debug.h"
#include <config-messagelist.h>

#include "MessageCore/StringUtil"
#include <Akonadi/Item>
#include <Akonadi/MessageStatus>
#include <MessageCore/DateFormatter>

#include <KLocalizedString>

#include <QApplication>
#include <QDateTime>
#include <QElapsedTimer>
#include <QIcon>
#include <QLocale>
#include <QScrollBar>
#include <QTimer>

#include <algorithm>
#include <chrono>

using namespace std::chrono_literals;

namespace MessageList
{
namespace Core
{
Q_GLOBAL_STATIC(QTimer, _k_heartBeatTimer)

/**
 * A job in a "View Fill" or "View Cleanup" or "View Update" task.
 *
 * For a "View Fill" task a job is a set of messages
 * that are contiguous in the storage. The set is expressed as a range
 * of row indexes. The task "sweeps" the storage in the specified
 * range, creates the appropriate Item instances and places them
 * in the right position in the tree.
 *
 * The idea is that in a single instance and for the same StorageModel
 * the jobs should never "cover" the same message twice. This assertion
 * is enforced all around this source file.
 *
 * For a "View Cleanup" task the job is a list of ModelInvariantIndex
 * objects (that are in fact MessageItem objects) that need to be removed
 * from the view.
 *
 * For a "View Update" task the job is a list of ModelInvariantIndex
 * objects (that are in fact MessageItem objects) that need to be updated.
 *
 * The interesting fact is that all the tasks need
 * very similar operations to be performed on the message tree.
 *
 * For a "View Fill" we have 5 passes.
 *
 * Pass 1 scans the underlying storage, creates the MessageItem objects
 * (which are subclasses of ModelInvariantIndex) and retrieves invariant
 * storage indexes for them. It also builds threading caches and
 * attempts to do some "easy" threading. If it succeeds in threading
 * and some conditions apply then it also attaches the items to the view.
 * Any unattached message is placed in a list.
 *
 * Pass 2 scans the list of messages that haven't been attached in
 * the first pass and performs perfect and reference based threading.
 * Since grouping of messages may depend on the "shape" of the thread
 * then certain threads aren't attached to the view yet.
 * Unassigned messages get stuffed into a list waiting for Pass3
 * or directly to a list waiting for Pass4 (that is, Pass3 may be skipped
 * if there is no hope to find an imperfect parent by subject based threading).
 *
 * Pass 3 scans the list of messages that haven't been attached in
 * the first and second passes and performs subject based threading.
 * Since grouping of messages may depend on the "shape" of the thread
 * then certain threads aren't attached to the view yet.
 * Anything unattached gets stuffed into the list waiting for Pass4.
 *
 * Pass 4 scans the unattached threads and puts them in the appropriate
 * groups. After this pass nothing is unattached.
 *
 * Pass 5 eventually re-sorts the groups and removes the empty ones.
 *
 * For a "View Cleanup" we still have 5 passes.
 *
 * Pass 1 scans the list of invalidated ModelInvariantIndex-es, casts
 * them to MessageItem objects and detaches them from the view.
 * The orphan children of the destroyed items get stuffed in the list
 * of unassigned messages that has been used also in the "View Fill" task above.
 *
 * Pass 2, 3, 4 and 5: same as "View Fill", just operating on the "orphaned"
 * messages that need to be reattached to the view.
 *
 * For a "View Update" we still have 5 passes.
 *
 * Pass 1 scans the list of ModelInvariantIndex-es that need an update, casts
 * them to MessageItem objects and handles the updates from storage.
 * The updates may cause a regrouping so items might be stuffed in one
 * of the lists for pass 4 or 5.
 *
 * Pass 2, 3 and 4 are simply empty.
 *
 * Pass 5: same as "View Fill", just operating on groups that require updates
 * after the messages have been moved in pass 1.
 *
 * That's why we in fact have Pass1Fill, Pass1Cleanup, Pass1Update, Pass2, Pass3, Pass4 and Pass5 below.
 * Pass1Fill, Pass1Cleanup and Pass1Update are exclusive and all of them proceed with Pass2 when finished.
 */
class ViewItemJob
{
public:
    enum Pass {
        Pass1Fill = 0, ///< Build threading caches, *TRY* to do some threading, try to attach something to the view
        Pass1Cleanup = 1, ///< Kill messages, build list of orphans
        Pass1Update = 2, ///< Update messages
        Pass2 = 3, ///< Thread everything by using caches, try to attach more to the view
        Pass3 = 4, ///< Do more threading (this time try to guess), try to attach more to the view
        Pass4 = 5, ///< Attach anything is still unattached
        Pass5 = 6, ///< Eventually Re-sort group headers and remove the empty ones
        LastIndex = 7 ///< Keep this at the end, needed to get the size of the enum
    };

private:
    // Data for "View Fill" jobs
    int mStartIndex; ///< The first index (in the underlying storage) of this job
    int mCurrentIndex; ///< The current index (in the underlying storage) of this job
    int mEndIndex; ///< The last index (in the underlying storage) of this job

    // Data for "View Cleanup" jobs
    QList<ModelInvariantIndex *> *mInvariantIndexList; ///< Owned list of shallow pointers

    // Common data

    // The maximum time that we can spend "at once" inside viewItemJobStep() (milliseconds)
    // The bigger this value, the larger chunks of work we do at once and less the time
    // we lose in "breaking and resuming" the job. On the other hand, large values tend
    // to make the view less responsive up to a "freeze" perception if this value is larger
    // than 2000.
    int mChunkTimeout;

    // The interval between two fillView steps. The larger the interval, the more interactivity
    // we have. The shorter the interval, the more work we get done per second.
    int mIdleInterval;

    // The minimum number of messages we process in every viewItemJobStep() call
    // The larger this value, the less time we lose in checking the timeout every N messages.
    // On the other hand, making this very large may make the view less responsive
    // if we're processing very few messages at a time and very high values (say > 10000) may
    // eventually make our job un-cancelable.
    int mMessageCheckCount;
    Pass mCurrentPass;

    // If this parameter is true then this job uses a "disconnected" UI.
    // It's FAR faster since we don't need to call beginInsertRows()/endInsertRows()
    // and we simply Q_EMIT a layoutChanged() at the end. It can be done only as the first
    // job though: subsequent jobs can't use layoutChanged() as it loses the expanded
    // state of items.
    bool mDisconnectUI;

public:
    /**
     * Creates a "View Fill" operation job
     */
    ViewItemJob(int startIndex, int endIndex, int chunkTimeout, int idleInterval, int messageCheckCount, bool disconnectUI = false)
        : mStartIndex(startIndex)
        , mCurrentIndex(startIndex)
        , mEndIndex(endIndex)
        , mInvariantIndexList(nullptr)
        , mChunkTimeout(chunkTimeout)
        , mIdleInterval(idleInterval)
        , mMessageCheckCount(messageCheckCount)
        , mCurrentPass(Pass1Fill)
        , mDisconnectUI(disconnectUI)
    {
    }

    /**
     * Creates a "View Cleanup" or "View Update" operation job
     */
    ViewItemJob(Pass pass, QList<ModelInvariantIndex *> *invariantIndexList, int chunkTimeout, int idleInterval, int messageCheckCount)
        : mStartIndex(0)
        , mCurrentIndex(0)
        , mEndIndex(invariantIndexList->count() - 1)
        , mInvariantIndexList(invariantIndexList)
        , mChunkTimeout(chunkTimeout)
        , mIdleInterval(idleInterval)
        , mMessageCheckCount(messageCheckCount)
        , mCurrentPass(pass)
        , mDisconnectUI(false)
    {
    }

    ~ViewItemJob()
    {
        delete mInvariantIndexList;
    }

public:
    [[nodiscard]] int startIndex() const
    {
        return mStartIndex;
    }

    void setStartIndex(int startIndex)
    {
        mStartIndex = startIndex;
        mCurrentIndex = startIndex;
    }

    [[nodiscard]] int currentIndex() const
    {
        return mCurrentIndex;
    }

    void setCurrentIndex(int currentIndex)
    {
        mCurrentIndex = currentIndex;
    }

    [[nodiscard]] int endIndex() const
    {
        return mEndIndex;
    }

    void setEndIndex(int endIndex)
    {
        mEndIndex = endIndex;
    }

    [[nodiscard]] Pass currentPass() const
    {
        return mCurrentPass;
    }

    void setCurrentPass(Pass pass)
    {
        mCurrentPass = pass;
    }

    [[nodiscard]] int idleInterval() const
    {
        return mIdleInterval;
    }

    [[nodiscard]] int chunkTimeout() const
    {
        return mChunkTimeout;
    }

    [[nodiscard]] int messageCheckCount() const
    {
        return mMessageCheckCount;
    }

    [[nodiscard]] QList<ModelInvariantIndex *> *invariantIndexList() const
    {
        return mInvariantIndexList;
    }

    [[nodiscard]] bool disconnectUI() const
    {
        return mDisconnectUI;
    }
};
} // namespace Core
} // namespace MessageList

using namespace MessageList::Core;

Model::Model(View *pParent)
    : QAbstractItemModel(pParent)
    , d(new ModelPrivate(this))
{
    d->mRecursionCounterForReset = 0;
    d->mStorageModel = nullptr;
    d->mView = pParent;
    d->mAggregation = nullptr;
    d->mTheme = nullptr;
    d->mSortOrder = nullptr;
    d->mFilter = nullptr;
    d->mPersistentSetManager = nullptr;
    d->mInLengthyJobBatch = false;
    d->mLastSelectedMessageInFolder = nullptr;
    d->mLoading = false;

    d->mRootItem = new Item(Item::InvisibleRoot);
    d->mRootItem->setViewable(nullptr, true);

    d->mFillStepTimer.setSingleShot(true);
    d->mInvariantRowMapper = new ModelInvariantRowMapper();
    d->mModelForItemFunctions = this;
    connect(&d->mFillStepTimer, &QTimer::timeout, this, [this]() {
        d->viewItemJobStep();
    });

    d->mCachedTodayLabel = i18n("Today");
    d->mCachedYesterdayLabel = i18n("Yesterday");
    d->mCachedUnknownLabel = i18nc("Unknown date", "Unknown");
    d->mCachedLastWeekLabel = i18n("Last Week");
    d->mCachedTwoWeeksAgoLabel = i18n("Two Weeks Ago");
    d->mCachedThreeWeeksAgoLabel = i18n("Three Weeks Ago");
    d->mCachedFourWeeksAgoLabel = i18n("Four Weeks Ago");
    d->mCachedFiveWeeksAgoLabel = i18n("Five Weeks Ago");

    d->mCachedWatchedOrIgnoredStatusBits = Akonadi::MessageStatus::statusIgnored().toQInt32() | Akonadi::MessageStatus::statusWatched().toQInt32();

    connect(_k_heartBeatTimer(), &QTimer::timeout, this, [this]() {
        d->checkIfDateChanged();
    });

    if (!_k_heartBeatTimer->isActive()) { // First model starts it
        _k_heartBeatTimer->start(1min); // 1 minute
    }
}

Model::~Model()
{
    setStorageModel(nullptr);

    d->clearJobList();
    d->mOldestItem = nullptr;
    d->mNewestItem = nullptr;
    d->clearUnassignedMessageLists();
    d->clearOrphanChildrenHash();
    d->clearThreadingCacheReferencesIdMD5ToMessageItem();
    d->clearThreadingCacheMessageSubjectMD5ToMessageItem();
    delete d->mPersistentSetManager;
    // Delete the invariant row mapper before removing the items.
    // It's faster since the items will not need to call the invariant
    delete d->mInvariantRowMapper;
    delete d->mRootItem;
}

void Model::setAggregation(const Aggregation *aggregation)
{
    d->mAggregation = aggregation;
    d->mView->setRootIsDecorated((d->mAggregation->grouping() == Aggregation::NoGrouping) && (d->mAggregation->threading() != Aggregation::NoThreading));
}

void Model::setTheme(const Theme *theme)
{
    d->mTheme = theme;
}

void Model::setSortOrder(const SortOrder *sortOrder)
{
    d->mSortOrder = sortOrder;
}

const SortOrder *Model::sortOrder() const
{
    return d->mSortOrder;
}

void Model::setFilter(const Filter *filter)
{
    d->mFilter = filter;

    if (d->mFilter) {
        connect(d->mFilter, &Filter::finished, this, [this]() {
            d->slotApplyFilter();
        });
    }

    d->slotApplyFilter();
}

void ModelPrivate::slotApplyFilter()
{
    auto childList = mRootItem->childItems();
    if (!childList) {
        return;
    }

    QModelIndex idx; // invalid

    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (const auto child : std::as_const(*childList)) {
        applyFilterToSubtree(child, idx);
    }

    QApplication::restoreOverrideCursor();
}

bool ModelPrivate::applyFilterToSubtree(Item *item, const QModelIndex &parentIndex)
{
    // This function applies the current filter (eventually empty)
    // to a message tree starting at "item".

    if (!mModelForItemFunctions) {
        qCWarning(MESSAGELIST_LOG) << "Cannot apply filter, the UI must be not disconnected.";
        return true;
    }
    Q_ASSERT(item); // the item must obviously be valid
    Q_ASSERT(item->isViewable()); // the item must be viewable

    // Apply to children first

    auto childList = item->childItems();

    bool childrenMatch = false;

    QModelIndex thisIndex = q->index(item, 0);

    if (childList) {
        for (const auto child : std::as_const(*childList)) {
            if (applyFilterToSubtree(child, thisIndex)) {
                childrenMatch = true;
            }
        }
    }

    if (!mFilter) { // empty filter always matches (but does not expand items)
        mView->setRowHidden(thisIndex.row(), parentIndex, false);
        return true;
    }

    if (childrenMatch) {
        mView->setRowHidden(thisIndex.row(), parentIndex, false);

        if (!mView->isExpanded(thisIndex)) {
            expandViewNoninteractive(thisIndex);
        }
        return true;
    }

    if (item->type() == Item::Message) {
        if (mFilter->match((MessageItem *)item)) {
            mView->setRowHidden(thisIndex.row(), parentIndex, false);
            return true;
        }
    } // else this is a group header and it never explicitly matches

    // filter doesn't match, hide the item
    mView->setRowHidden(thisIndex.row(), parentIndex, true);

    return false;
}

int Model::columnCount(const QModelIndex &parent) const
{
    if (!d->mTheme) {
        return 0;
    }
    if (parent.column() > 0) {
        return 0;
    }
    return d->mTheme->columns().count();
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    /// this is called only when Akonadi is using the selectionmodel
    ///  for item actions. since akonadi uses the ETM ItemRoles, and the
    ///  messagelist uses its own internal roles, here we respond
    ///  to the ETM ones.

    auto item = static_cast<Item *>(index.internalPointer());

    switch (role) {
    /// taken from entitytreemodel.h
    case Qt::UserRole + 1: // EntityTreeModel::ItemIdRole
        if (item->type() == MessageList::Core::Item::Message) {
            auto mItem = static_cast<MessageItem *>(item);
            return QVariant::fromValue(mItem->akonadiItem().id());
        } else {
            return {};
        }
        break;
    case Qt::UserRole + 2: // EntityTreeModel::ItemRole
        if (item->type() == MessageList::Core::Item::Message) {
            auto mItem = static_cast<MessageItem *>(item);
            return QVariant::fromValue(mItem->akonadiItem());
        } else {
            return {};
        }
        break;
    case Qt::UserRole + 3: // EntityTreeModel::MimeTypeRole
        if (item->type() == MessageList::Core::Item::Message) {
            return u"message/rfc822"_s;
        } else {
            return {};
        }
        break;
    case Qt::AccessibleTextRole:
        if (item->type() == MessageList::Core::Item::Message) {
            auto mItem = static_cast<MessageItem *>(item);
            return mItem->accessibleText(d->mTheme, index.column());
        } else if (item->type() == MessageList::Core::Item::GroupHeader) {
            if (index.column() > 0) {
                return QString();
            }
            auto hItem = static_cast<GroupHeaderItem *>(item);
            return hItem->label();
        }
        return QString();
        break;
    default:
        return {};
    }
}

QVariant Model::headerData(int section, Qt::Orientation, int role) const
{
    if (!d->mTheme) {
        return {};
    }

    auto column = d->mTheme->column(section);
    if (!column) {
        return {};
    }

    if (d->mStorageModel && column->isSenderOrReceiver() && (role == Qt::DisplayRole)) {
        if (d->mStorageModelContainsOutboundMessages) {
            return QVariant(i18n("Receiver"));
        }
        return QVariant(i18n("Sender"));
    }

    const bool columnPixmapEmpty(column->pixmapName().isEmpty());
    if ((role == Qt::DisplayRole) && columnPixmapEmpty) {
        return QVariant(column->label());
    } else if ((role == Qt::ToolTipRole) && !columnPixmapEmpty) {
        return QVariant(column->label());
    } else if ((role == Qt::DecorationRole) && !columnPixmapEmpty) {
        return QVariant(QIcon::fromTheme(column->pixmapName()));
    }

    return {};
}

QModelIndex Model::index(Item *item, int column) const
{
    if (!d->mModelForItemFunctions) {
        return {}; // called with disconnected UI: the item isn't known on the Qt side, yet
    }

    if (!item) {
        return {};
    }
    // FIXME: This function is a bottleneck (the caching in indexOfChildItem only works 30% of the time)
    auto par = item->parent();
    if (!par) {
        if (item != d->mRootItem) {
            item->dump(QString());
        }
        return {};
    }

    const int index = par->indexOfChildItem(item);
    if (index < 0) {
        return {}; // BUG
    }
    return createIndex(index, column, item);
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const
{
    if (!d->mModelForItemFunctions) {
        return {}; // called with disconnected UI: the item isn't known on the Qt side, yet
    }

#ifdef READD_THIS_IF_YOU_WANT_TO_PASS_MODEL_TEST
    if (column < 0) {
        return QModelIndex(); // senseless column (we could optimize by skipping this check but ModelTest from trolltech is pedantic)
    }
#endif

    const Item *item;
    if (parent.isValid()) {
        item = static_cast<const Item *>(parent.internalPointer());
        if (!item) {
            return {}; // should never happen
        }
    } else {
        item = d->mRootItem;
    }

    if (parent.column() > 0) {
        return {}; // parent column is not 0: shouldn't have children (as per Qt documentation)
    }

    Item *child = item->childItem(row);
    if (!child) {
        return {}; // no such row in parent
    }
    return createIndex(row, column, child);
}

QModelIndex Model::parent(const QModelIndex &modelIndex) const
{
    Q_ASSERT(d->mModelForItemFunctions); // should be never called with disconnected UI

    if (!modelIndex.isValid()) {
        return {}; // should never happen
    }
    auto item = static_cast<Item *>(modelIndex.internalPointer());
    if (!item) {
        return {};
    }
    auto par = item->parent();
    if (!par) {
        return {}; // should never happen
    }
    // return index( par, modelIndex.column() );
    return index(par, 0); // parents are always in column 0 (as per Qt documentation)
}

int Model::rowCount(const QModelIndex &parent) const
{
    if (!d->mModelForItemFunctions) {
        return 0; // called with disconnected UI
    }

    const Item *item;
    if (parent.isValid()) {
        item = static_cast<const Item *>(parent.internalPointer());
        if (!item) {
            return 0; // should never happen
        }
    } else {
        item = d->mRootItem;
    }

    if (!item->isViewable()) {
        return 0;
    }

    return item->childItemCount();
}

class RecursionPreventer
{
public:
    RecursionPreventer(int &counter)
        : mCounter(counter)
    {
        mCounter++;
    }

    ~RecursionPreventer()
    {
        mCounter--;
    }

    [[nodiscard]] bool isRecursive() const
    {
        return mCounter > 1;
    }

private:
    int &mCounter;
};

StorageModel *Model::storageModel() const
{
    return d->mStorageModel;
}

void ModelPrivate::clear()
{
    q->beginResetModel();
    if (mFillStepTimer.isActive()) {
        mFillStepTimer.stop();
    }

    // Kill pre-selection at this stage
    mPreSelectionMode = PreSelectNone;
    mLastSelectedMessageInFolder = nullptr;
    mOldestItem = nullptr;
    mNewestItem = nullptr;

    // Reset the row mapper before removing items
    // This is faster since the items don't need to access the mapper.
    mInvariantRowMapper->modelReset();

    clearJobList();
    clearUnassignedMessageLists();
    clearOrphanChildrenHash();
    mGroupHeaderItemHash.clear();
    mGroupHeadersThatNeedUpdate.clear();
    mThreadingCacheMessageIdMD5ToMessageItem.clear();
    mThreadingCacheMessageInReplyToIdMD5ToMessageItem.clear();
    clearThreadingCacheReferencesIdMD5ToMessageItem();
    clearThreadingCacheMessageSubjectMD5ToMessageItem();
    mViewItemJobStepChunkTimeout = 100;
    mViewItemJobStepIdleInterval = 10;
    mViewItemJobStepMessageCheckCount = 10;
    delete mPersistentSetManager;
    mPersistentSetManager = nullptr;
    mCurrentItemToRestoreAfterViewItemJobStep = nullptr;

    mTodayDate = QDate::currentDate();

    // FIXME: CLEAR THE FILTER HERE AS WE CAN'T APPLY IT WITH UI DISCONNECTED!

    mRootItem->killAllChildItems();

    q->endResetModel();
    // Q_EMIT headerDataChanged();

    mView->selectionModel()->clearSelection();
}

void ModelPrivate::expandViewNoninteractive(const QModelIndex &index)
{
    // isAnimated() and setAnimated() are cheap: they just get or set one boolean
    const bool animatedSave = mView->isAnimated();
    // Animations, however, are expensive: each one caches a QPixmap for smooth rendering
    mView->setAnimated(false);
    mView->expand(index);
    mView->setAnimated(animatedSave);
}

void Model::setStorageModel(StorageModel *storageModel, PreSelectionMode preSelectionMode)
{
    // Prevent a case of recursion when opening a folder that has a message and the folder was
    // never opened before.
    RecursionPreventer preventer(d->mRecursionCounterForReset);
    if (preventer.isRecursive()) {
        return;
    }

    d->clear();

    if (d->mStorageModel) {
        // Disconnect all signals from old storageModel
        std::for_each(d->mStorageModelConnections.cbegin(), d->mStorageModelConnections.cend(), [](const QMetaObject::Connection &c) -> bool {
            return QObject::disconnect(c);
        });
        d->mStorageModelConnections.clear();
    }

    const bool isReload = (d->mStorageModel == storageModel);
    d->mStorageModel = storageModel;

    if (!d->mStorageModel) {
        return; // no folder: nothing to fill
    }

    // Save threading cache of the previous folder, but only if the cache was
    // enabled and a different folder is being loaded - reload of the same folder
    // means change in aggregation in which case we will have to re-build the
    // cache so there's no point saving the current threading cache.
    if (d->mThreadingCache.isEnabled() && !isReload) {
        d->mThreadingCache.save();
    } else {
        if (isReload) {
            qCDebug(MESSAGELIST_LOG) << "Identical folder reloaded, not saving old threading cache";
        } else {
            qCDebug(MESSAGELIST_LOG) << "Threading disabled in previous folder, not saving threading cache";
        }
    }
    // Load threading cache for the new folder, but only if threading is enabled,
    // otherwise we would just be caching a flat list.
    if (d->mAggregation->threading() != Aggregation::NoThreading) {
        d->mThreadingCache.setEnabled(true);
        d->mThreadingCache.load(d->mStorageModel->id(), d->mAggregation);
    } else {
        // No threading, no cache - don't even bother inserting entries into the
        // cache or trying to look them up there
        d->mThreadingCache.setEnabled(false);
        qCDebug(MESSAGELIST_LOG) << "Threading disabled in folder" << d->mStorageModel->id() << ", not using threading cache";
    }

    d->mPreSelectionMode = preSelectionMode;
    d->mStorageModelContainsOutboundMessages = d->mStorageModel->containsOutboundMessages();

    // clang-format off
    d->mStorageModelConnections = {
        connect(d->mStorageModel,  &StorageModel::rowsInserted,
                this, [this](const QModelIndex &parent, int first, int last) {
            d->slotStorageModelRowsInserted(parent, first, last);
        }),
        connect(d->mStorageModel, &StorageModel::rowsRemoved,
                this, [this](const QModelIndex &parent, int first, int last) {
            d->slotStorageModelRowsRemoved(parent, first, last);
        }),
        connect(d->mStorageModel, &StorageModel::layoutChanged,
                this, [this]() {
            d->slotStorageModelLayoutChanged();
        }),
        connect(d->mStorageModel, &StorageModel::modelReset,
                this, [this]() {
            d->slotStorageModelLayoutChanged();
        }),
        connect(d->mStorageModel, &StorageModel::dataChanged,
                this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight) {
            d->slotStorageModelDataChanged(topLeft, bottomRight);
        }),
        connect(d->mStorageModel, &StorageModel::headerDataChanged,
                this, [this](Qt::Orientation orientation, int first, int last) {
            d->slotStorageModelHeaderDataChanged(orientation, first, last);
        })
    };
    // clang-format on

    if (d->mStorageModel->rowCount() == 0) {
        return; // folder empty: nothing to fill
    }

    // Here we use different strategies based on user preference and the folder size.
    // The knobs we can tune are:
    //
    // - The number of jobs used to scan the whole folder and their order
    //
    //   There are basically two approaches to this. One is the "single big job"
    //   approach. It scans the folder from the beginning to the end in a single job
    //   entry. The job passes are done only once. It's advantage is that it's simpler
    //   and it's less likely to generate imperfect parent threadings. The bad
    //   side is that since the folders are "sort of" date ordered then the most interesting
    //   messages show up at the end of the work. Not nice for large folders.
    //   The other approach uses two jobs. This is a bit slower but smarter strategy.
    //   First we scan the latest 1000 messages and *then* take care of the older ones.
    //   This will show up the most interesting messages almost immediately. (Well...
    //   All this assuming that the underlying storage always appends the newly arrived messages)
    //   The strategy is slower since it  generates some imperfect parent threadings which must be
    //   adjusted by the second job. For instance, in my kernel mailing list folder this "smart" approach
    //   generates about 150 additional imperfectly threaded children... but the "today"
    //   messages show up almost immediately. The two-chunk job also makes computing
    //   the percentage user feedback a little harder and might break some optimization
    //   in the insertions (we're able to optimize appends and prepends but a chunked
    //   job is likely to split our work at a boundary where messages are always inserted
    //   in the middle of the list).
    //
    // - The maximum time to spend inside a single job step
    //
    //   The larger this time, the greater the number of messages per second that this
    //   engine can process but also greater time with frozen UI -> less interactivity.
    //   Reasonable values start at 50 msecs. Values larger than 300 msecs are very likely
    //   to be perceived by the user as UI non-reactivity.
    //
    // - The number of messages processed in each job step subchunk.
    //
    //   A job subchunk is processed without checking the maximum time above. This means
    //   that each job step will process at least the number of messages specified by this value.
    //   Very low values mean that we respect the maximum time very carefully but we also
    //   waste time to check if we ran out of time :)
    //   Very high values are likely to cause the engine to not respect the maximum step time.
    //   Reasonable values go from 5 to 100.
    //
    // - The "idle" time between two steps
    //
    //   The lower this time, the greater the number of messages per second that this
    //   engine can process but also lower time for the UI to process events -> less interactivity.
    //   A value of 0 here means that Qt will trigger the timer as soon as it has some
    //   idle time to spend. UI events will be still processed but slowdowns are possible.
    //   0 is reasonable though. Values larger than 200 will tend to make the total job
    //   completion times high.
    //

    // If we have no filter it seems that we can apply a huge optimization.
    // We disconnect the UI for the first huge filling job. This allows us
    // to save the extremely expensive beginInsertRows()/endInsertRows() calls
    // and call a single layoutChanged() at the end. This slows down a lot item
    // expansion. But on the other side if only few items need to be expanded
    // then this strategy is better. If filtering is enabled then this strategy
    // isn't applicable (because filtering requires interaction with the UI
    // while the data is loading).

    // So...

    // For the very first small chunk it's ok to work with disconnected UI as long
    // as we have no filter. The first small chunk is always 1000 messages, so
    // even if all of them are expanded, it's still somewhat acceptable.
    bool canDoFirstSmallChunkWithDisconnectedUI = !d->mFilter;

    // Larger works need a bigger condition: few messages must be expanded in the end.
    bool canDoJobWithDisconnectedUI = // we have no filter
        !d->mFilter
        && (
            // we do no threading at all
            (d->mAggregation->threading() == Aggregation::NoThreading) || // or we never expand threads
            (d->mAggregation->threadExpandPolicy() == Aggregation::NeverExpandThreads) || // or we expand threads but we'll be going to expand really only a few
            (
                // so we don't expand them all
                (d->mAggregation->threadExpandPolicy() != Aggregation::AlwaysExpandThreads) && // and we'd expand only a few in fact
                (d->mStorageModel->initialUnreadRowCountGuess() < 1000)));

    switch (d->mAggregation->fillViewStrategy()) {
    case Aggregation::FavorInteractivity:
        // favor interactivity
        if ((!canDoJobWithDisconnectedUI) && (d->mStorageModel->rowCount() > 3000)) { // empiric value
            // First a small job with the most recent messages. Large chunk, small (but non zero) idle interval
            // and a larger number of messages to process at once.
            auto job1 =
                new ViewItemJob(d->mStorageModel->rowCount() - 1000, d->mStorageModel->rowCount() - 1, 200, 20, 100, canDoFirstSmallChunkWithDisconnectedUI);
            d->mViewItemJobs.append(job1);
            // Then a larger job with older messages. Small chunk, bigger idle interval, small number of messages to
            // process at once.
            auto job2 = new ViewItemJob(0, d->mStorageModel->rowCount() - 1001, 100, 50, 10, false);
            d->mViewItemJobs.append(job2);

            // We could even extremize this by splitting the folder in several
            // chunks and scanning them from the newest to the oldest... but the overhead
            // due to imperfectly threaded children would be probably too big.
        } else {
            // small folder or can be done with disconnected UI: single chunk work.
            // Lag the CPU a bit more but not too much to destroy even the earliest interactivity.
            auto job = new ViewItemJob(0, d->mStorageModel->rowCount() - 1, 150, 30, 30, canDoJobWithDisconnectedUI);
            d->mViewItemJobs.append(job);
        }
        break;
    case Aggregation::FavorSpeed:
        // More batchy jobs, still interactive to a certain degree
        if ((!canDoJobWithDisconnectedUI) && (d->mStorageModel->rowCount() > 3000)) { // empiric value
            // large folder, but favor speed
            auto job1 =
                new ViewItemJob(d->mStorageModel->rowCount() - 1000, d->mStorageModel->rowCount() - 1, 250, 0, 100, canDoFirstSmallChunkWithDisconnectedUI);
            d->mViewItemJobs.append(job1);
            auto job2 = new ViewItemJob(0, d->mStorageModel->rowCount() - 1001, 200, 0, 10, false);
            d->mViewItemJobs.append(job2);
        } else {
            // small folder or can be done with disconnected UI and favor speed: single chunk work.
            // Lag the CPU more, get more work done
            auto job = new ViewItemJob(0, d->mStorageModel->rowCount() - 1, 250, 0, 100, canDoJobWithDisconnectedUI);
            d->mViewItemJobs.append(job);
        }
        break;
    case Aggregation::BatchNoInteractivity: {
        // one large job, never interrupt, block UI
        auto job = new ViewItemJob(0, d->mStorageModel->rowCount() - 1, 60000, 0, 100000, canDoJobWithDisconnectedUI);
        d->mViewItemJobs.append(job);
        break;
    }
    default:
        qCWarning(MESSAGELIST_LOG) << "Unrecognized fill view strategy";
        Q_ASSERT(false);
        break;
    }

    d->mLoading = true;

    d->viewItemJobStep();
}

void ModelPrivate::checkIfDateChanged()
{
    // This function is called by MessageList::Core::Manager once in a while (every 1 minute or sth).
    // It is used to check if the current date has changed (with respect to mTodayDate).
    //
    // Our message items cache the formatted dates (as formatting them
    // on the fly would be too expensive). We also cache the labels of the groups which often display dates.
    // When the date changes we would need to fix all these strings.
    //
    // A dedicated algorithm to refresh the labels of the items would be either too complex
    // or would block on large trees. Fixing the labels of the groups is also quite hard...
    //
    // So to keep the things simple we just reload the view.

    if (!mStorageModel) {
        return; // nothing to do
    }

    if (mLoading) {
        return; // not now
    }

    if (!mViewItemJobs.isEmpty()) {
        return; // not now
    }

    if (mTodayDate == QDate::currentDate()) {
        return; // date not changed
    }

    // date changed, reload the view (and try to preserve the current selection)
    Manager::instance()->dateFormatter()->invalidateReferenceDate();
    q->setStorageModel(mStorageModel, PreSelectLastSelected);
}

void Model::setPreSelectionMode(PreSelectionMode preSelect)
{
    d->mPreSelectionMode = preSelect;
    d->mLastSelectedMessageInFolder = nullptr;
}

//
// The "view fill" algorithm implemented in the functions below is quite smart but also quite complex.
// It's governed by the following goals:
//
// - Be flexible: allow different configurations from "unsorted flat list" to a "grouped and threaded
//     list with different sorting algorithms applied to each aggregation level"
// - Be reasonably fast
// - Be non blocking: UI shouldn't freeze while the algorithm is running
// - Be interruptible: user must be able to abort the execution and just switch to another folder in the middle
//

void ModelPrivate::clearUnassignedMessageLists()
{
    // This is a bit tricky...
    // The three unassigned message lists contain messages that have been created
    // but not yet attached to the view. There may be two major cases for a message:
    // - it has no parent -> it must be deleted and it will delete its children too
    // - it has a parent -> it must NOT be deleted since it will be deleted by its parent.

    // Sometimes the things get a little complicated since in Pass2 and Pass3
    // we have transitional states in that the MessageItem object can be in two of these lists.

    // WARNING: This function does NOT fixup mNewestItem and mOldestItem. If one of these
    // two messages is in the lists below, it's deleted and the member becomes a dangling pointer.
    // The caller must ensure that both mNewestItem and mOldestItem are set to 0
    // and this is enforced in the assert below to avoid errors. This basically means
    // that this function should be called only when the storage model changes or
    // when the model is destroyed.
    Q_ASSERT((mOldestItem == nullptr) && (mNewestItem == nullptr));

    if (!mUnassignedMessageListForPass2.isEmpty()) {
        // We're actually in Pass1* or Pass2: everything is mUnassignedMessageListForPass2
        // Something may *also* be in mUnassignedMessageListForPass3 and mUnassignedMessageListForPass4
        // but that are duplicates for sure.

        // We can't just sweep the list and delete parentless items since each delete
        // could kill children which are somewhere AFTER in the list: accessing the children
        // would then lead to a SIGSEGV. We first sweep the list gathering parentless
        // items and *then* delete them without accessing the parented ones.

        QList<MessageItem *> parentless;
        for (const auto mi : std::as_const(mUnassignedMessageListForPass2)) {
            if (!mi->parent()) {
                parentless.append(mi);
            }
        }

        for (const auto mi : std::as_const(parentless)) {
            delete mi;
        }

        mUnassignedMessageListForPass2.clear();
        // Any message these list contain was also in mUnassignedMessageListForPass2
        mUnassignedMessageListForPass3.clear();
        mUnassignedMessageListForPass4.clear();
        return;
    }

    // mUnassignedMessageListForPass2 is empty

    if (!mUnassignedMessageListForPass3.isEmpty()) {
        // We're actually at the very end of Pass2 or inside Pass3
        // Pass2 pushes stuff in mUnassignedMessageListForPass3 *or* mUnassignedMessageListForPass4
        // Pass3 pushes stuff from mUnassignedMessageListForPass3 to mUnassignedMessageListForPass4
        // So if we're in Pass2 then the two lists contain distinct messages but if we're in Pass3
        // then the two lists may contain the same messages.

        if (!mUnassignedMessageListForPass4.isEmpty()) {
            // We're actually in Pass3: the messiest one.

            QSet<MessageItem *> itemsToDelete;
            for (const auto mi : std::as_const(mUnassignedMessageListForPass3)) {
                if (!mi->parent()) {
                    itemsToDelete.insert(mi);
                }
            }
            for (const auto mi : std::as_const(mUnassignedMessageListForPass4)) {
                if (!mi->parent()) {
                    itemsToDelete.insert(mi);
                }
            }
            for (const auto mi : std::as_const(itemsToDelete)) {
                delete mi;
            }

            mUnassignedMessageListForPass3.clear();
            mUnassignedMessageListForPass4.clear();
            return;
        }

        // mUnassignedMessageListForPass4 is empty so we must be at the end of a very special kind of Pass2
        // We have the same problem as in mUnassignedMessageListForPass2.
        QList<MessageItem *> parentless;
        for (const auto mi : std::as_const(mUnassignedMessageListForPass3)) {
            if (!mi->parent()) {
                parentless.append(mi);
            }
        }
        for (const auto mi : std::as_const(parentless)) {
            delete mi;
        }

        mUnassignedMessageListForPass3.clear();
        return;
    }

    // mUnassignedMessageListForPass3 is empty
    if (!mUnassignedMessageListForPass4.isEmpty()) {
        // we're in Pass4.. this is easy.

        // We have the same problem as in mUnassignedMessageListForPass2.
        QList<MessageItem *> parentless;
        for (const auto mi : std::as_const(mUnassignedMessageListForPass4)) {
            if (!mi->parent()) {
                parentless.append(mi);
            }
        }
        for (const auto mi : std::as_const(parentless)) {
            delete mi;
        }

        mUnassignedMessageListForPass4.clear();
        return;
    }
}

void ModelPrivate::clearThreadingCacheReferencesIdMD5ToMessageItem()
{
    qDeleteAll(mThreadingCacheMessageReferencesIdMD5ToMessageItem);
    mThreadingCacheMessageReferencesIdMD5ToMessageItem.clear();
}

void ModelPrivate::clearThreadingCacheMessageSubjectMD5ToMessageItem()
{
    qDeleteAll(mThreadingCacheMessageSubjectMD5ToMessageItem);
    mThreadingCacheMessageSubjectMD5ToMessageItem.clear();
}

void ModelPrivate::clearOrphanChildrenHash()
{
    qDeleteAll(mOrphanChildrenHash);
    mOrphanChildrenHash.clear();
}

void ModelPrivate::clearJobList()
{
    if (mViewItemJobs.isEmpty()) {
        return;
    }

    if (mInLengthyJobBatch) {
        mInLengthyJobBatch = false;
    }

    qDeleteAll(mViewItemJobs);
    mViewItemJobs.clear();

    mModelForItemFunctions = q; // make sure it's true, as there remains no job with disconnected UI
}

void ModelPrivate::attachGroup(GroupHeaderItem *ghi)
{
    if (ghi->parent()) {
        if (((ghi)->childItemCount() > 0) // has children
            && (ghi)->isViewable() // is actually attached to the viewable root
            && mModelForItemFunctions // the UI is not disconnected
            && mView->isExpanded(q->index(ghi, 0)) // is actually expanded
        ) {
            saveExpandedStateOfSubtree(ghi);
        }

        // FIXME: This *WILL* break selection and current index... :/

        ghi->parent()->takeChildItem(mModelForItemFunctions, ghi);
    }

    ghi->setParent(mRootItem);

    // I'm using a macro since it does really improve readability.
    // I'm NOT using a helper function since gcc will refuse to inline some of
    // the calls because they make this function grow too much.
#define INSERT_GROUP_WITH_COMPARATOR(_ItemComparator)                                                                                                          \
    switch (mSortOrder->groupSortDirection()) {                                                                                                                \
    case SortOrder::Ascending:                                                                                                                                 \
        mRootItem->d_ptr->insertChildItem<_ItemComparator, true>(mModelForItemFunctions, ghi);                                                                 \
        break;                                                                                                                                                 \
    case SortOrder::Descending:                                                                                                                                \
        mRootItem->d_ptr->insertChildItem<_ItemComparator, false>(mModelForItemFunctions, ghi);                                                                \
        break;                                                                                                                                                 \
    default: /* should never happen... */                                                                                                                      \
        mRootItem->appendChildItem(mModelForItemFunctions, ghi);                                                                                               \
        break;                                                                                                                                                 \
    }

    switch (mSortOrder->groupSorting()) {
    case SortOrder::SortGroupsByDateTime:
        INSERT_GROUP_WITH_COMPARATOR(ItemDateComparator)
        break;
    case SortOrder::SortGroupsByDateTimeOfMostRecent:
        INSERT_GROUP_WITH_COMPARATOR(ItemMaxDateComparator)
        break;
    case SortOrder::SortGroupsBySenderOrReceiver:
        INSERT_GROUP_WITH_COMPARATOR(ItemSenderOrReceiverComparator)
        break;
    case SortOrder::SortGroupsBySender:
        INSERT_GROUP_WITH_COMPARATOR(ItemSenderComparator)
        break;
    case SortOrder::SortGroupsByReceiver:
        INSERT_GROUP_WITH_COMPARATOR(ItemReceiverComparator)
        break;
    case SortOrder::NoGroupSorting:
        mRootItem->appendChildItem(mModelForItemFunctions, ghi);
        break;
    default: // should never happen
        mRootItem->appendChildItem(mModelForItemFunctions, ghi);
        break;
    }

    if (ghi->initialExpandStatus() == Item::ExpandNeeded) { // this actually is a "non viewable expanded state"
        if (ghi->childItemCount() > 0) {
            if (mModelForItemFunctions) { // the UI is not disconnected
                syncExpandedStateOfSubtree(ghi);
            }
        }
    }

    // A group header is always viewable, when attached: apply the filter, if we have it.
    if (mFilter) {
        Q_ASSERT(mModelForItemFunctions); // UI must be NOT disconnected
        // apply the filter to subtree
        applyFilterToSubtree(ghi, QModelIndex());
    }
}

void ModelPrivate::saveExpandedStateOfSubtree(Item *root)
{
    Q_ASSERT(mModelForItemFunctions); // UI must be NOT disconnected here
    Q_ASSERT(root);

    root->setInitialExpandStatus(Item::ExpandNeeded);

    auto children = root->childItems();
    if (!children) {
        return;
    }
    for (const auto mi : std::as_const(*children)) {
        if (mi->childItemCount() > 0 // has children
            && mi->isViewable() // is actually attached to the viewable root
            && mView->isExpanded(q->index(mi, 0))) { // is actually expanded
            saveExpandedStateOfSubtree(mi);
        }
    }
}

void ModelPrivate::syncExpandedStateOfSubtree(Item *root)
{
    Q_ASSERT(mModelForItemFunctions); // UI must be NOT disconnected here

    // WE ASSUME that:
    // - the item is viewable
    // - its initialExpandStatus() is Item::ExpandNeeded
    // - it has at least one children (well.. this is not a strict requirement, but it's a waste of resources to expand items that don't have children)

    QModelIndex idx = q->index(root, 0);

    // if ( !mView->isExpanded( idx ) ) // this is O(logN!) in Qt.... very ugly... but it should never happen here
    expandViewNoninteractive(idx); // sync the real state in the view
    root->setInitialExpandStatus(Item::ExpandExecuted);

    auto children = root->childItems();
    if (!children) {
        return;
    }

    for (const auto mi : std::as_const(*children)) {
        if (mi->initialExpandStatus() == Item::ExpandNeeded) {
            if (mi->childItemCount() > 0) {
                syncExpandedStateOfSubtree(mi);
            }
        }
    }
}

void ModelPrivate::attachMessageToGroupHeader(MessageItem *mi)
{
    QString groupLabel;
    time_t date;

    // compute the group header label and the date
    switch (mAggregation->grouping()) {
    case Aggregation::GroupByDate:
    case Aggregation::GroupByDateRange: {
        if (mAggregation->threadLeader() == Aggregation::MostRecentMessage) {
            date = mi->maxDate();
        } else {
            date = mi->date();
        }

        QDateTime dt;
        dt.setSecsSinceEpoch(date);
        QDate dDate = dt.date();
        int daysAgo = -1;
        const int daysInWeek = 7;
        if (dDate.isValid() && mTodayDate.isValid()) {
            daysAgo = dDate.daysTo(mTodayDate);
        }

        if ((daysAgo < 0) // In the future
            || (static_cast<uint>(date) == static_cast<uint>(-1))) { // Invalid
            groupLabel = mCachedUnknownLabel;
        } else if (daysAgo == 0) { // Today
            groupLabel = mCachedTodayLabel;
        } else if (daysAgo == 1) { // Yesterday
            groupLabel = mCachedYesterdayLabel;
        } else if (daysAgo > 1 && daysAgo < daysInWeek) { // Within last seven days
            auto dayName = mCachedDayNameLabel.find(dDate.dayOfWeek()); // non-const call, but non-shared container
            if (dayName == mCachedDayNameLabel.end()) {
                dayName = mCachedDayNameLabel.insert(dDate.dayOfWeek(), QLocale::system().standaloneDayName(dDate.dayOfWeek()));
            }
            groupLabel = *dayName;
        } else if (mAggregation->grouping() == Aggregation::GroupByDate) { // GroupByDate seven days or more ago
            groupLabel = QLocale::system().toString(dDate, QLocale::ShortFormat);
        } else if (dDate.month() == mTodayDate.month() // GroupByDateRange within this month
                   && dDate.year() == mTodayDate.year()) {
            const int startOfWeekDaysAgo = (daysInWeek + mTodayDate.dayOfWeek() - QLocale().firstDayOfWeek()) % daysInWeek;
            const int weeksAgo = ((daysAgo - startOfWeekDaysAgo) / daysInWeek) + 1;
            switch (weeksAgo) {
            case 0: // This week
                groupLabel = QLocale::system().standaloneDayName(dDate.dayOfWeek());
                break;
            case 1: // 1 week ago
                groupLabel = mCachedLastWeekLabel;
                break;
            case 2:
                groupLabel = mCachedTwoWeeksAgoLabel;
                break;
            case 3:
                groupLabel = mCachedThreeWeeksAgoLabel;
                break;
            case 4:
                groupLabel = mCachedFourWeeksAgoLabel;
                break;
            case 5:
                groupLabel = mCachedFiveWeeksAgoLabel;
                break;
            default: // should never happen
                groupLabel = mCachedUnknownLabel;
            }
        } else if (dDate.year() == mTodayDate.year()) { // GroupByDateRange within this year
            auto monthName = mCachedMonthNameLabel.find(dDate.month()); // non-const call, but non-shared container
            if (monthName == mCachedMonthNameLabel.end()) {
                monthName = mCachedMonthNameLabel.insert(dDate.month(), QLocale::system().standaloneMonthName(dDate.month()));
            }
            groupLabel = *monthName;
        } else { // GroupByDateRange in previous years
            auto monthName = mCachedMonthNameLabel.find(dDate.month()); // non-const call, but non-shared container
            if (monthName == mCachedMonthNameLabel.end()) {
                monthName = mCachedMonthNameLabel.insert(dDate.month(), QLocale::system().standaloneMonthName(dDate.month()));
            }
            groupLabel = i18nc("Message Aggregation Group Header: Month name and Year number",
                               "%1 %2",
                               *monthName,
                               QLocale::system().toString(dDate, QLatin1StringView("yyyy")));
        }
        break;
    }

    case Aggregation::GroupBySenderOrReceiver:
        date = mi->date();
        groupLabel = mi->displaySenderOrReceiver();
        break;

    case Aggregation::GroupBySender:
        date = mi->date();
        groupLabel = mi->displaySender();
        break;

    case Aggregation::GroupByReceiver:
        date = mi->date();
        groupLabel = mi->displayReceiver();
        break;

    case Aggregation::NoGrouping:
        // append directly to root
        attachMessageToParent(mRootItem, mi);
        return;

    default:
        // should never happen
        attachMessageToParent(mRootItem, mi);
        return;
    }

    GroupHeaderItem *ghi;

    ghi = mGroupHeaderItemHash.value(groupLabel, nullptr);
    if (!ghi) {
        // not found

        ghi = new GroupHeaderItem(groupLabel);
        ghi->initialSetup(date, mi->size(), mi->sender(), mi->receiver(), mi->useReceiver());

        switch (mAggregation->groupExpandPolicy()) {
        case Aggregation::NeverExpandGroups:
            // nothing to do
            break;
        case Aggregation::AlwaysExpandGroups:
            // expand always
            ghi->setInitialExpandStatus(Item::ExpandNeeded);
            break;
        case Aggregation::ExpandRecentGroups:
            // expand only if "close" to today
            if (mViewItemJobStepStartTime > ghi->date()) {
                if ((mViewItemJobStepStartTime - ghi->date()) < (3600 * 72)) {
                    ghi->setInitialExpandStatus(Item::ExpandNeeded);
                }
            } else {
                if ((ghi->date() - mViewItemJobStepStartTime) < (3600 * 72)) {
                    ghi->setInitialExpandStatus(Item::ExpandNeeded);
                }
            }
            break;
        default:
            // b0rken
            break;
        }

        attachMessageToParent(ghi, mi);

        attachGroup(ghi); // this will expand the group if required

        mGroupHeaderItemHash.insert(groupLabel, ghi);
    } else {
        // the group was already there (certainly viewable)

        // This function may be also called to re-group a message.
        // That is, to eventually find a new group for a message that has changed
        // its properties (but was already attached to a group).
        // So it may happen that we find out that in fact re-grouping wasn't really
        // needed because the message is already in the correct group.
        if (mi->parent() == ghi) {
            return; // nothing to be done
        }

        attachMessageToParent(ghi, mi);
    }

    // Remember this message as a thread leader
    mThreadingCache.updateParent(mi, nullptr);
}

MessageItem *ModelPrivate::findMessageParent(MessageItem *mi)
{
    Q_ASSERT(mAggregation->threading() != Aggregation::NoThreading); // caller must take care of this

    // This function attempts to find a thread parent for the item "mi"
    // which actually may already have a children subtree.

    // Forged or plain broken message trees are dangerous here.
    // For example, a message tree with circular references like
    //
    //        Message mi, Id=1, In-Reply-To=2
    //          Message childOfMi, Id=2, In-Reply-To=1
    //
    // is perfectly possible and will cause us to find childOfMi
    // as parent of mi. This will then create a loop in the message tree
    // (which will then no longer be a tree in fact) and cause us to freeze
    // once we attempt to climb the parents. We need to take care of that.

    bool bMessageWasThreadable = false;
    MessageItem *pParent;

    // First of all try to find a "perfect parent", that is the message for that
    // we have the ID in the "In-Reply-To" field. This is actually done by using
    // MD5 caches of the message ids because of speed. Collisions are very unlikely.

    MD5Hash md5 = mi->inReplyToIdMD5();
    if (!md5.isEmpty()) {
        // have an In-Reply-To field MD5
        pParent = mThreadingCacheMessageIdMD5ToMessageItem.value(md5, nullptr);
        if (pParent) {
            // Take care of circular references
            if ((mi == pParent) // self referencing message
                || ((mi->childItemCount() > 0) // mi already has children, this is fast to determine
                    && pParent->hasAncestor(mi) // pParent is in the mi's children tree
                    )) {
                qCWarning(MESSAGELIST_LOG) << "Circular In-Reply-To reference loop detected in the message tree";
                mi->setThreadingStatus(MessageItem::NonThreadable);
                return nullptr; // broken message: throw it away
            }
            mi->setThreadingStatus(MessageItem::PerfectParentFound);
            return pParent; // got a perfect parent for this message
        }

        // got no perfect parent
        bMessageWasThreadable = true; // but the message was threadable
    }

    if (mAggregation->threading() == Aggregation::PerfectOnly) {
        mi->setThreadingStatus(bMessageWasThreadable ? MessageItem::ParentMissing : MessageItem::NonThreadable);
        return nullptr; // we're doing only perfect parent matches
    }

    // Try to use the "References" field. In fact we have the MD5 of the
    // (n-1)th entry in References.
    //
    // Original rationale from KMHeaders:
    //
    // If we don't have a replyToId, or if we have one and the
    // corresponding message is not in this folder, as happens
    // if you keep your outgoing messages in an OUTBOX, for
    // example, try the list of references, because the second
    // to last will likely be in this folder. replyToAuxIdMD5
    // contains the second to last one.

    md5 = mi->referencesIdMD5();
    if (!md5.isEmpty()) {
        pParent = mThreadingCacheMessageIdMD5ToMessageItem.value(md5, nullptr);
        if (pParent) {
            // Take care of circular references
            if ((mi == pParent) // self referencing message
                || ((mi->childItemCount() > 0) // mi already has children, this is fast to determine
                    && pParent->hasAncestor(mi) // pParent is in the mi's children tree
                    )) {
                qCWarning(MESSAGELIST_LOG) << "Circular reference loop detected in the message tree";
                mi->setThreadingStatus(MessageItem::NonThreadable);
                return nullptr; // broken message: throw it away
            }
            mi->setThreadingStatus(MessageItem::ImperfectParentFound);
            return pParent; // got an imperfect parent for this message
        }

        auto messagesWithTheSameReferences = mThreadingCacheMessageReferencesIdMD5ToMessageItem.value(md5, nullptr);
        if (messagesWithTheSameReferences) {
            Q_ASSERT(!messagesWithTheSameReferences->isEmpty());

            pParent = messagesWithTheSameReferences->first();
            if (mi != pParent && (mi->childItemCount() == 0 || !pParent->hasAncestor(mi))) {
                mi->setThreadingStatus(MessageItem::ImperfectParentFound);
                return pParent;
            }
        }

        // got no imperfect parent
        bMessageWasThreadable = true; // but the message was threadable
    }

    if (mAggregation->threading() == Aggregation::PerfectAndReferences) {
        mi->setThreadingStatus(bMessageWasThreadable ? MessageItem::ParentMissing : MessageItem::NonThreadable);
        return nullptr; // we're doing only perfect parent matches
    }

    Q_ASSERT(mAggregation->threading() == Aggregation::PerfectReferencesAndSubject);

    // We are supposed to do subject based threading but we can't do it now.
    // This is because the subject based threading *may* be wrong and waste
    // time by creating circular references (that we'd need to detect and fix).
    // We first try the perfect and references based threading on all the messages
    // and then run subject based threading only on the remaining ones.

    mi->setThreadingStatus((bMessageWasThreadable || mi->subjectIsPrefixed()) ? MessageItem::ParentMissing : MessageItem::NonThreadable);
    return nullptr;
}

// Subject threading cache stuff

#if 0
// Debug helpers
void dump_iterator_and_list(QList< MessageItem * >::Iterator &iter, QList< MessageItem * > *list)
{
    qCDebug(MESSAGELIST_LOG) << "Threading cache part dump";
    if (iter == list->end()) {
        qCDebug(MESSAGELIST_LOG) << "Iterator pointing to end of the list";
    } else {
        qCDebug(MESSAGELIST_LOG) << "Iterator pointing to " << *iter << " subject [" << (*iter)->subject() << "] date [" << (*iter)->date() << "]";
    }

    for (QList< MessageItem * >::Iterator it = list->begin(); it != list->end(); ++it) {
        qCDebug(MESSAGELIST_LOG) << "List element " << *it << " subject [" << (*it)->subject() << "] date [" << (*it)->date() << "]";
    }

    qCDebug(MESSAGELIST_LOG) << "End of threading cache part dump";
}

void dump_list(QList< MessageItem * > *list)
{
    qCDebug(MESSAGELIST_LOG) << "Threading cache part dump";

    for (QList< MessageItem * >::Iterator it = list->begin(); it != list->end(); ++it) {
        qCDebug(MESSAGELIST_LOG) << "List element " << *it << " subject [" << (*it)->subject() << "] date [" << (*it)->date() << "]";
    }

    qCDebug(MESSAGELIST_LOG) << "End of threading cache part dump";
}

#endif // debug helpers

// a helper class used in a qLowerBound() call below
class MessageLessThanByDate
{
public:
    inline bool operator()(const MessageItem *mi1, const MessageItem *mi2) const
    {
        if (mi1->date() < mi2->date()) { // likely
            return true;
        }
        if (mi1->date() > mi2->date()) { // likely
            return false;
        }
        // dates are equal, compare by pointer
        return mi1 < mi2;
    }
};

void ModelPrivate::addMessageToReferencesBasedThreadingCache(MessageItem *mi)
{
    // Messages in this cache are sorted by date, and if dates are equal then they are sorted by pointer value.
    // Sorting by date is used to optimize the parent lookup in guessMessageParent() below.

    // WARNING: If the message date changes for some reason (like in the "update" step)
    //          then the cache may become unsorted. For this reason the message about to
    //          be changed must be first removed from the cache and then reinserted.

    auto messagesWithTheSameReference = mThreadingCacheMessageReferencesIdMD5ToMessageItem.value(mi->referencesIdMD5(), nullptr);

    if (!messagesWithTheSameReference) {
        messagesWithTheSameReference = new QList<MessageItem *>();
        mThreadingCacheMessageReferencesIdMD5ToMessageItem.insert(mi->referencesIdMD5(), messagesWithTheSameReference);
        messagesWithTheSameReference->append(mi);
        return;
    }

    // Found: assert that we have no duplicates in the cache.
    Q_ASSERT(!messagesWithTheSameReference->contains(mi));

    // Ordered insert: first by date then by pointer value.
    auto it = std::lower_bound(messagesWithTheSameReference->begin(), messagesWithTheSameReference->end(), mi, MessageLessThanByDate());
    messagesWithTheSameReference->insert(it, mi);
}

void ModelPrivate::removeMessageFromReferencesBasedThreadingCache(MessageItem *mi)
{
    // We assume that the caller knows what he is doing and the message is actually in the cache.
    // If the message isn't in the cache then we should not be called at all.

    auto messagesWithTheSameReference = mThreadingCacheMessageReferencesIdMD5ToMessageItem.value(mi->referencesIdMD5(), nullptr);

    // We assume that the message is there so the list must be non null.
    Q_ASSERT(messagesWithTheSameReference);

    // The cache *MUST* be ordered first by date then by pointer value
    auto it = std::lower_bound(messagesWithTheSameReference->begin(), messagesWithTheSameReference->end(), mi, MessageLessThanByDate());

    // The binary based search must have found a message
    Q_ASSERT(it != messagesWithTheSameReference->end());

    // and it must have found exactly the message requested
    Q_ASSERT(*it == mi);

    // Kill it
    messagesWithTheSameReference->erase(it);

    // And kill the list if it was the last one
    if (messagesWithTheSameReference->isEmpty()) {
        mThreadingCacheMessageReferencesIdMD5ToMessageItem.remove(mi->referencesIdMD5());
        delete messagesWithTheSameReference;
    }
}

void ModelPrivate::addMessageToSubjectBasedThreadingCache(MessageItem *mi)
{
    // Messages in this cache are sorted by date, and if dates are equal then they are sorted by pointer value.
    // Sorting by date is used to optimize the parent lookup in guessMessageParent() below.

    // WARNING: If the message date changes for some reason (like in the "update" step)
    //          then the cache may become unsorted. For this reason the message about to
    //          be changed must be first removed from the cache and then reinserted.

    // Lookup the list of messages with the same stripped subject
    auto messagesWithTheSameStrippedSubject = mThreadingCacheMessageSubjectMD5ToMessageItem.value(mi->strippedSubjectMD5(), nullptr);

    if (!messagesWithTheSameStrippedSubject) {
        // Not there yet: create it and append.
        messagesWithTheSameStrippedSubject = new QList<MessageItem *>();
        mThreadingCacheMessageSubjectMD5ToMessageItem.insert(mi->strippedSubjectMD5(), messagesWithTheSameStrippedSubject);
        messagesWithTheSameStrippedSubject->append(mi);
        return;
    }

    // Found: assert that we have no duplicates in the cache.
    Q_ASSERT(!messagesWithTheSameStrippedSubject->contains(mi));

    // Ordered insert: first by date then by pointer value.
    auto it = std::lower_bound(messagesWithTheSameStrippedSubject->begin(), messagesWithTheSameStrippedSubject->end(), mi, MessageLessThanByDate());
    messagesWithTheSameStrippedSubject->insert(it, mi);
}

void ModelPrivate::removeMessageFromSubjectBasedThreadingCache(MessageItem *mi)
{
    // We assume that the caller knows what he is doing and the message is actually in the cache.
    // If the message isn't in the cache then we should not be called at all.
    //
    // The game is called "performance"

    // Grab the list of all the messages with the same stripped subject (all potential parents)
    auto messagesWithTheSameStrippedSubject = mThreadingCacheMessageSubjectMD5ToMessageItem.value(mi->strippedSubjectMD5(), nullptr);

    // We assume that the message is there so the list must be non null.
    Q_ASSERT(messagesWithTheSameStrippedSubject);

    // The cache *MUST* be ordered first by date then by pointer value
    auto it = std::lower_bound(messagesWithTheSameStrippedSubject->begin(), messagesWithTheSameStrippedSubject->end(), mi, MessageLessThanByDate());

    // The binary based search must have found a message
    Q_ASSERT(it != messagesWithTheSameStrippedSubject->end());

    // and it must have found exactly the message requested
    Q_ASSERT(*it == mi);

    // Kill it
    messagesWithTheSameStrippedSubject->erase(it);

    // And kill the list if it was the last one
    if (messagesWithTheSameStrippedSubject->isEmpty()) {
        mThreadingCacheMessageSubjectMD5ToMessageItem.remove(mi->strippedSubjectMD5());
        delete messagesWithTheSameStrippedSubject;
    }
}

MessageItem *ModelPrivate::guessMessageParent(MessageItem *mi)
{
    // This function implements subject based threading
    // It attempts to guess a thread parent for the item "mi"
    // which actually may already have a children subtree.

    // We have all the problems of findMessageParent() plus the fact that
    // we're actually guessing (and often we may be *wrong*).

    Q_ASSERT(mAggregation->threading() == Aggregation::PerfectReferencesAndSubject); // caller must take care of this
    Q_ASSERT(mi->subjectIsPrefixed()); // caller must take care of this
    Q_ASSERT(mi->threadingStatus() == MessageItem::ParentMissing);

    // Do subject based threading
    const auto md5 = mi->strippedSubjectMD5();
    if (!md5.isEmpty()) {
        auto messagesWithTheSameStrippedSubject = mThreadingCacheMessageSubjectMD5ToMessageItem.value(md5, nullptr);

        if (messagesWithTheSameStrippedSubject) {
            Q_ASSERT(!messagesWithTheSameStrippedSubject->isEmpty());

            // Need to find the message with the maximum date lower than the one of this message

            auto maxTime = (time_t)0;
            MessageItem *pParent = nullptr;

            // Here'we re really guessing so circular references are possible
            // even on perfectly valid trees. This is why we don't consider it
            // an error but just continue searching.

            // FIXME: This might be speed up with an initial binary search (?)
            // ANSWER: No. We can't rely on date order (as it can be updated on the fly...)
            for (const auto it : std::as_const(*messagesWithTheSameStrippedSubject)) {
                int delta = mi->date() - it->date();

                // We don't take into account messages with a delta smaller than 120.
                // Assuming that our date() values are correct (that is, they take into
                // account timezones etc..) then one usually needs more than 120 seconds
                // to answer to a message. Better safe than sorry.

                // This check also includes negative deltas so messages later than mi aren't considered

                if (delta < 120) {
                    break; // The list is ordered by date (ascending) so we can stop searching here
                }

                // About the "magic" 3628899 value here comes a Till's comment from the original KMHeaders:
                //
                //   "Parents more than six weeks older than the message are not accepted. The reasoning being
                //   that if a new message with the same subject turns up after such a long time, the chances
                //   that it is still part of the same thread are slim. The value of six weeks is chosen as a
                //   result of a poll conducted on kde-devel, so it's probably bogus. :)"

                if (delta < 3628899) {
                    // Compute the closest.
                    if ((maxTime < it->date())) {
                        // This algorithm *can* be (and often is) wrong.
                        // Take care of circular threading which is really possible at this level.
                        // If mi contains "it" inside its children subtree then we have
                        // found such a circular threading problem.

                        // Note that here we can't have it == mi because of the delta >= 120 check above.

                        if ((mi->childItemCount() == 0) || !it->hasAncestor(mi)) {
                            maxTime = it->date();
                            pParent = it;
                        }
                    }
                }
            }

            if (pParent) {
                mi->setThreadingStatus(MessageItem::ImperfectParentFound);
                return pParent; // got an imperfect parent for this message
            }
        }
    }

    return nullptr;
}

//
// A little template helper, hopefully inlineable.
//
// Return true if the specified message item is in the wrong position
// inside the specified parent and needs re-sorting. Return false otherwise.
// Both parent and messageItem must not be null.
//
// Checking if a message needs re-sorting instead of just re-sorting it
// is very useful since re-sorting is an expensive operation.
//
template<class ItemComparator>
static bool messageItemNeedsReSorting(SortOrder::SortDirection messageSortDirection, ItemPrivate *parent, MessageItem *messageItem)
{
    if ((messageSortDirection == SortOrder::Ascending) || (parent->mType == Item::Message)) {
        return parent->childItemNeedsReSorting<ItemComparator, true>(messageItem);
    }
    return parent->childItemNeedsReSorting<ItemComparator, false>(messageItem);
}

bool ModelPrivate::handleItemPropertyChanges(int propertyChangeMask, Item *parent, Item *item)
{
    // The facts:
    //
    // - If dates changed:
    //   - If we're sorting messages by min/max date then at each level the messages might need resorting.
    //   - If the thread leader is the most recent message of a thread then the uppermost
    //     message of the thread might need re-grouping.
    //   - If the groups are sorted by min/max date then the group might need re-sorting too.
    //
    // This function explicitly doesn't re-apply the filter when ActionItemStatus changes.
    // This is because filters must be re-applied due to a broader range of status variations:
    // this is done in viewItemJobStepInternalForJobPass1Update() instead (which is the only
    // place in that ActionItemStatus may be set).

    if (parent->type() == Item::InvisibleRoot) {
        // item is either a message or a group attached to the root.
        // It might need resorting.
        if (item->type() == Item::GroupHeader) {
            // item is a group header attached to the root.
            if ((
                    // max date changed
                    (propertyChangeMask & MaxDateChanged) && // groups sorted by max date
                    (mSortOrder->groupSorting() == SortOrder::SortGroupsByDateTimeOfMostRecent))
                || (
                    // date changed
                    (propertyChangeMask & DateChanged) && // groups sorted by date
                    (mSortOrder->groupSorting() == SortOrder::SortGroupsByDateTime))) {
                // This group might need re-sorting.

                // Groups are large container of messages so it's likely that
                // another message inserted will cause this group to be marked again.
                // So we wait until the end to do the grand final re-sorting: it will be done in Pass4.
                mGroupHeadersThatNeedUpdate.insert(static_cast<GroupHeaderItem *>(item), static_cast<GroupHeaderItem *>(item));
            }
        } else {
            // item is a message. It might need re-sorting.

            // Since sorting is an expensive operation, we first check if it's *really* needed.
            // Re-sorting will actually not change min/max dates at all and
            // will not climb up the parent's ancestor tree.

            switch (mSortOrder->messageSorting()) {
            case SortOrder::SortMessagesByDateTime:
                if (propertyChangeMask & DateChanged) { // date changed
                    if (messageItemNeedsReSorting<ItemDateComparator>(mSortOrder->messageSortDirection(), parent->d_ptr, static_cast<MessageItem *>(item))) {
                        attachMessageToParent(parent, static_cast<MessageItem *>(item));
                    }
                } // else date changed, but it doesn't match sorting order: no need to re-sort
                break;
            case SortOrder::SortMessagesByDateTimeOfMostRecent:
                if (propertyChangeMask & MaxDateChanged) { // max date changed
                    if (messageItemNeedsReSorting<ItemMaxDateComparator>(mSortOrder->messageSortDirection(), parent->d_ptr, static_cast<MessageItem *>(item))) {
                        attachMessageToParent(parent, static_cast<MessageItem *>(item));
                    }
                } // else max date changed, but it doesn't match sorting order: no need to re-sort
                break;
            case SortOrder::SortMessagesByActionItemStatus:
                if (propertyChangeMask & ActionItemStatusChanged) { // todo status changed
                    if (messageItemNeedsReSorting<ItemActionItemStatusComparator>(mSortOrder->messageSortDirection(),
                                                                                  parent->d_ptr,
                                                                                  static_cast<MessageItem *>(item))) {
                        attachMessageToParent(parent, static_cast<MessageItem *>(item));
                    }
                } // else to do status changed, but it doesn't match sorting order: no need to re-sort
                break;
            case SortOrder::SortMessagesByUnreadStatus:
                if (propertyChangeMask & UnreadStatusChanged) { // new / unread status changed
                    if (messageItemNeedsReSorting<ItemUnreadStatusComparator>(mSortOrder->messageSortDirection(),
                                                                              parent->d_ptr,
                                                                              static_cast<MessageItem *>(item))) {
                        attachMessageToParent(parent, static_cast<MessageItem *>(item));
                    }
                } // else new/unread status changed, but it doesn't match sorting order: no need to re-sort
                break;
            case SortOrder::SortMessagesByImportantStatus:
                if (propertyChangeMask & ImportantStatusChanged) { // important status changed
                    if (messageItemNeedsReSorting<ItemImportantStatusComparator>(mSortOrder->messageSortDirection(),
                                                                                 parent->d_ptr,
                                                                                 static_cast<MessageItem *>(item))) {
                        attachMessageToParent(parent, static_cast<MessageItem *>(item));
                    }
                } // else new/unread status changed, but it doesn't match sorting order: no need to re-sort
                break;
            case SortOrder::SortMessagesByAttachmentStatus:
                if (propertyChangeMask & AttachmentStatusChanged) { // attachment status changed
                    if (messageItemNeedsReSorting<ItemAttachmentStatusComparator>(mSortOrder->messageSortDirection(),
                                                                                  parent->d_ptr,
                                                                                  static_cast<MessageItem *>(item))) {
                        attachMessageToParent(parent, static_cast<MessageItem *>(item));
                    }
                } // else new/unread status changed, but it doesn't match sorting order: no need to re-sort
                break;
            default:
                // this kind of message sorting isn't affected by the property changes: nothing to do.
                break;
            }
        }

        return false; // the invisible root isn't affected by any change.
    }

    if (parent->type() == Item::GroupHeader) {
        // item is a message attached to a GroupHeader.
        // It might need re-grouping or re-sorting (within the same group)

        // Check re-grouping here.
        if ((
                // max date changed
                (propertyChangeMask & MaxDateChanged) && // thread leader is most recent message
                (mAggregation->threadLeader() == Aggregation::MostRecentMessage))
            || (
                // date changed
                (propertyChangeMask & DateChanged) && // thread leader the topmost message
                (mAggregation->threadLeader() == Aggregation::TopmostMessage))) {
            // Might really need re-grouping.
            // attachMessageToGroupHeader() will find the right group for this message
            // and if it's different than the current it will move it.
            attachMessageToGroupHeader(static_cast<MessageItem *>(item));
            // Re-grouping fixes the properties of the involved group headers
            // so at exit of attachMessageToGroupHeader() the parent can't be affected
            // by the change anymore.
            return false;
        }

        // Re-grouping wasn't needed. Re-sorting might be.
    } // else item is a message attached to another message and might need re-sorting only.

    // Check if message needs re-sorting.

    switch (mSortOrder->messageSorting()) {
    case SortOrder::SortMessagesByDateTime:
        if (propertyChangeMask & DateChanged) { // date changed
            if (messageItemNeedsReSorting<ItemDateComparator>(mSortOrder->messageSortDirection(), parent->d_ptr, static_cast<MessageItem *>(item))) {
                attachMessageToParent(parent, static_cast<MessageItem *>(item));
            }
        } // else date changed, but it doesn't match sorting order: no need to re-sort
        break;
    case SortOrder::SortMessagesByDateTimeOfMostRecent:
        if (propertyChangeMask & MaxDateChanged) { // max date changed
            if (messageItemNeedsReSorting<ItemMaxDateComparator>(mSortOrder->messageSortDirection(), parent->d_ptr, static_cast<MessageItem *>(item))) {
                attachMessageToParent(parent, static_cast<MessageItem *>(item));
            }
        } // else max date changed, but it doesn't match sorting order: no need to re-sort
        break;
    case SortOrder::SortMessagesByActionItemStatus:
        if (propertyChangeMask & ActionItemStatusChanged) { // todo status changed
            if (messageItemNeedsReSorting<ItemActionItemStatusComparator>(mSortOrder->messageSortDirection(),
                                                                          parent->d_ptr,
                                                                          static_cast<MessageItem *>(item))) {
                attachMessageToParent(parent, static_cast<MessageItem *>(item));
            }
        } // else to do status changed, but it doesn't match sorting order: no need to re-sort
        break;
    case SortOrder::SortMessagesByUnreadStatus:
        if (propertyChangeMask & UnreadStatusChanged) { // new / unread status changed
            if (messageItemNeedsReSorting<ItemUnreadStatusComparator>(mSortOrder->messageSortDirection(), parent->d_ptr, static_cast<MessageItem *>(item))) {
                attachMessageToParent(parent, static_cast<MessageItem *>(item));
            }
        } // else new/unread status changed, but it doesn't match sorting order: no need to re-sort
        break;
    case SortOrder::SortMessagesByImportantStatus:
        if (propertyChangeMask & ImportantStatusChanged) { // important status changed
            if (messageItemNeedsReSorting<ItemImportantStatusComparator>(mSortOrder->messageSortDirection(), parent->d_ptr, static_cast<MessageItem *>(item))) {
                attachMessageToParent(parent, static_cast<MessageItem *>(item));
            }
        } // else important status changed, but it doesn't match sorting order: no need to re-sort
        break;
    case SortOrder::SortMessagesByAttachmentStatus:
        if (propertyChangeMask & AttachmentStatusChanged) { // attachment status changed
            if (messageItemNeedsReSorting<ItemAttachmentStatusComparator>(mSortOrder->messageSortDirection(),
                                                                          parent->d_ptr,
                                                                          static_cast<MessageItem *>(item))) {
                attachMessageToParent(parent, static_cast<MessageItem *>(item));
            }
        } // else important status changed, but it doesn't match sorting order: no need to re-sort
        break;
    default:
        // this kind of message sorting isn't affected by property changes: nothing to do.
        break;
    }

    return true; // parent might be affected too.
}

void ModelPrivate::messageDetachedUpdateParentProperties(Item *oldParent, MessageItem *mi)
{
    Q_ASSERT(oldParent);
    Q_ASSERT(mi);
    Q_ASSERT(oldParent != mRootItem);

    // oldParent might have its properties changed because of the child removal.
    // propagate the changes up.
    for (;;) {
        // pParent is not the root item now. This is assured by how we enter this loop
        // and by the fact that handleItemPropertyChanges returns false when grandParent
        // is Item::InvisibleRoot. We could actually assert it here...

        // Check if its dates need an update.
        int propertyChangeMask;

        if ((mi->maxDate() == oldParent->maxDate()) && oldParent->recomputeMaxDate()) {
            propertyChangeMask = MaxDateChanged;
        } else {
            break; // from the for(;;) loop
        }

        // One of the oldParent properties has changed for sure

        Item *grandParent = oldParent->parent();

        // If there is no grandParent then oldParent isn't attached to the view.
        // Re-sorting / re-grouping isn't needed for sure.
        if (!grandParent) {
            break; // from the for(;;) loop
        }

        // The following function will return true if grandParent may be affected by the change.
        // If the grandParent isn't affected, we stop climbing.
        if (!handleItemPropertyChanges(propertyChangeMask, grandParent, oldParent)) {
            break; // from the for(;;) loop
        }

        // Now we need to climb up one level and check again.
        oldParent = grandParent;
    } // for(;;) loop

    // If the last message was removed from a group header then this group will need an update
    // for sure. We will need to remove it (unless a message is attached back to it)
    if (oldParent->type() == Item::GroupHeader) {
        if (oldParent->childItemCount() == 0) {
            mGroupHeadersThatNeedUpdate.insert(static_cast<GroupHeaderItem *>(oldParent), static_cast<GroupHeaderItem *>(oldParent));
        }
    }
}

void ModelPrivate::propagateItemPropertiesToParent(Item *item)
{
    Item *pParent = item->parent();
    Q_ASSERT(pParent);
    Q_ASSERT(pParent != mRootItem);

    for (;;) {
        // pParent is not the root item now. This is assured by how we enter this loop
        // and by the fact that handleItemPropertyChanges returns false when grandParent
        // is Item::InvisibleRoot. We could actually assert it here...

        // Check if its dates need an update.
        int propertyChangeMask;

        if (item->maxDate() > pParent->maxDate()) {
            pParent->setMaxDate(item->maxDate());
            propertyChangeMask = MaxDateChanged;
        } else {
            // No parent dates have changed: no further work is needed. Stop climbing here.
            break; // from the for(;;) loop
        }

        // One of the pParent properties has changed.

        Item *grandParent = pParent->parent();

        // If there is no grandParent then pParent isn't attached to the view.
        // Re-sorting / re-grouping isn't needed for sure.
        if (!grandParent) {
            break; // from the for(;;) loop
        }

        // The following function will return true if grandParent may be affected by the change.
        // If the grandParent isn't affected, we stop climbing.
        if (!handleItemPropertyChanges(propertyChangeMask, grandParent, pParent)) {
            break; // from the for(;;) loop
        }

        // Now we need to climb up one level and check again.
        pParent = grandParent;
    } // for(;;)
}

void ModelPrivate::attachMessageToParent(Item *pParent, MessageItem *mi, AttachOptions attachOptions)
{
    Q_ASSERT(pParent);
    Q_ASSERT(mi);

    // This function may be called to do a simple "re-sort" of the item inside the parent.
    // In that case mi->parent() is equal to pParent.
    bool oldParentWasTheSame;

    if (mi->parent()) {
        Item *oldParent = mi->parent();

        // The item already had a parent and this means that we're moving it.
        oldParentWasTheSame = oldParent == pParent; // just re-sorting ?

        if (mi->isViewable()) { // is actually
            // The message is actually attached to the viewable root

            // Unfortunately we need to hack the model/view architecture
            // since it's somewhat flawed in this. At the moment of writing
            // there is simply no way to atomically move a subtree.
            // We must detach, call beginRemoveRows()/endRemoveRows(),
            // save the expanded state, save the selection, save the current item,
            // save the view position (YES! As we are removing items the view
            // will hopelessly jump around so we're just FORCED to break
            // the isolation from the view)...
            // ...*then* reattach, restore the expanded state, restore the selection,
            // restore the current item, restore the view position and pray
            // that nothing will fail in the (rather complicated) process....

            // Yet more unfortunately, while saving the expanded state might stop
            // at a certain (unexpanded) point in the tree, saving the selection
            // is hopelessly recursive down to the bare leafs.

            // Furthermore the expansion of items is a common case while selection
            // in the subtree is rare, so saving it would be a huge cost with
            // a low revenue.

            // This is why we just let the selection screw up. I hereby refuse to call
            // yet another expensive recursive function here :D

            // The current item saving can be somewhat optimized doing it once for
            // a single job step...

            if (((mi)->childItemCount() > 0) // has children
                && mModelForItemFunctions // the UI is not actually disconnected
                && mView->isExpanded(q->index(mi, 0)) // is actually expanded
            ) {
                saveExpandedStateOfSubtree(mi);
            }
        }

        // If the parent is viewable (so mi was viewable too) then the beginRemoveRows()
        // and endRemoveRows() functions of this model will be called too.
        oldParent->takeChildItem(mModelForItemFunctions, mi);

        if ((!oldParentWasTheSame) && (oldParent != mRootItem)) {
            messageDetachedUpdateParentProperties(oldParent, mi);
        }
    } else {
        // The item had no parent yet.
        oldParentWasTheSame = false;
    }

    // Take care of perfect / imperfect threading.
    // Items that are now perfectly threaded, but already have a different parent
    // might have been imperfectly threaded before. Remove them from the caches.
    // Items that are now imperfectly threaded must be added to the caches.
    //
    // If we're just re-sorting the item inside the same parent then the threading
    // caches don't need to be updated (since they actually depend on the parent).

    if (!oldParentWasTheSame) {
        switch (mi->threadingStatus()) {
        case MessageItem::PerfectParentFound:
            if (!mi->inReplyToIdMD5().isEmpty()) {
                mThreadingCacheMessageInReplyToIdMD5ToMessageItem.remove(mi->inReplyToIdMD5(), mi);
            }
            if (attachOptions == StoreInCache && pParent->type() == Item::Message) {
                mThreadingCache.updateParent(mi, static_cast<MessageItem *>(pParent));
            }
            break;
        case MessageItem::ImperfectParentFound:
        case MessageItem::ParentMissing: // may be: temporary or just fallback assignment
            if (!mi->inReplyToIdMD5().isEmpty()) {
                if (!mThreadingCacheMessageInReplyToIdMD5ToMessageItem.contains(mi->inReplyToIdMD5(), mi)) {
                    mThreadingCacheMessageInReplyToIdMD5ToMessageItem.insert(mi->inReplyToIdMD5(), mi);
                }
            }
            break;
        case MessageItem::NonThreadable: // this also happens when we do no threading at all
            // make gcc happy
            Q_ASSERT(!mThreadingCacheMessageInReplyToIdMD5ToMessageItem.contains(mi->inReplyToIdMD5(), mi));
            break;
        }
    }

    // Set the new parent
    mi->setParent(pParent);

    // Propagate watched and ignored status
    if ((pParent->status().toQInt32() & mCachedWatchedOrIgnoredStatusBits) // unlikely
        && (pParent->type() == Item::Message) // likely
    ) {
        // the parent is either watched or ignored: propagate to the child
        if (pParent->status().isWatched()) {
            int row = mInvariantRowMapper->modelInvariantIndexToModelIndexRow(mi);
            mi->setStatus(Akonadi::MessageStatus::statusWatched());
            mStorageModel->setMessageItemStatus(mi, row, Akonadi::MessageStatus::statusWatched());
        } else if (pParent->status().isIgnored()) {
            int row = mInvariantRowMapper->modelInvariantIndexToModelIndexRow(mi);
            mi->setStatus(Akonadi::MessageStatus::statusIgnored());
            mStorageModel->setMessageItemStatus(mi, row, Akonadi::MessageStatus::statusIgnored());
        }
    }

    // And insert into its child list

    // If pParent is viewable then the insert/append functions will call this model's
    // beginInsertRows() and endInsertRows() functions. This is EXTREMELY
    // expensive and ugly but it's the only way with the Qt4 imposed Model/View method.
    // Dude... (citation from Lost, if it wasn't clear).

    // I'm using a macro since it does really improve readability.
    // I'm NOT using a helper function since gcc will refuse to inline some of
    // the calls because they make this function grow too much.
#define INSERT_MESSAGE_WITH_COMPARATOR(_ItemComparator)                                                                                                        \
    if ((mSortOrder->messageSortDirection() == SortOrder::Ascending) || (pParent->type() == Item::Message)) {                                                  \
        pParent->d_ptr->insertChildItem<_ItemComparator, true>(mModelForItemFunctions, mi);                                                                    \
    } else {                                                                                                                                                   \
        pParent->d_ptr->insertChildItem<_ItemComparator, false>(mModelForItemFunctions, mi);                                                                   \
    }

    // If pParent is viewable then the insertion call will also set the child state to viewable.
    // Since mi MAY have children, then this call may make them viewable.
    switch (mSortOrder->messageSorting()) {
    case SortOrder::SortMessagesByDateTime:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemDateComparator)
        break;
    case SortOrder::SortMessagesByDateTimeOfMostRecent:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemMaxDateComparator)
        break;
    case SortOrder::SortMessagesBySize:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemSizeComparator)
        break;
    case SortOrder::SortMessagesBySenderOrReceiver:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemSenderOrReceiverComparator)
        break;
    case SortOrder::SortMessagesBySender:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemSenderComparator)
        break;
    case SortOrder::SortMessagesByReceiver:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemReceiverComparator)
        break;
    case SortOrder::SortMessagesBySubject:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemSubjectComparator)
        break;
    case SortOrder::SortMessagesByActionItemStatus:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemActionItemStatusComparator)
        break;
    case SortOrder::SortMessagesByUnreadStatus:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemUnreadStatusComparator)
        break;
    case SortOrder::SortMessagesByImportantStatus:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemImportantStatusComparator)
        break;
    case SortOrder::SortMessagesByAttachmentStatus:
        INSERT_MESSAGE_WITH_COMPARATOR(ItemAttachmentStatusComparator)
        break;
    case SortOrder::NoMessageSorting:
        pParent->appendChildItem(mModelForItemFunctions, mi);
        break;
    default: // should never happen
        pParent->appendChildItem(mModelForItemFunctions, mi);
        break;
    }

    // Decide if we need to expand parents
    bool childNeedsExpanding = (mi->initialExpandStatus() == Item::ExpandNeeded);

    if (pParent->initialExpandStatus() == Item::NoExpandNeeded) {
        switch (mAggregation->threadExpandPolicy()) {
        case Aggregation::NeverExpandThreads:
            // just do nothing unless this child has children and is already marked for expansion
            if (childNeedsExpanding) {
                pParent->setInitialExpandStatus(Item::ExpandNeeded);
            }
            break;
        case Aggregation::ExpandThreadsWithNewMessages: // No more new status. fall through to unread if it exists in config
        case Aggregation::ExpandThreadsWithUnreadMessages:
            // expand only if unread (or it has children marked for expansion)
            if (childNeedsExpanding || !mi->status().isRead()) {
                pParent->setInitialExpandStatus(Item::ExpandNeeded);
            }
            break;
        case Aggregation::ExpandThreadsWithUnreadOrImportantMessages:
            // expand only if unread, important or todo (or it has children marked for expansion)
            // FIXME: Wouldn't it be nice to be able to test for bitmasks in MessageStatus ?
            if (childNeedsExpanding || !mi->status().isRead() || mi->status().isImportant() || mi->status().isToAct()) {
                pParent->setInitialExpandStatus(Item::ExpandNeeded);
            }
            break;
        case Aggregation::AlwaysExpandThreads:
            // expand everything
            pParent->setInitialExpandStatus(Item::ExpandNeeded);
            break;
        default:
            // BUG
            break;
        }
    } // else it's already marked for expansion or expansion has been already executed

    // expand parent first, if possible
    if (pParent->initialExpandStatus() == Item::ExpandNeeded) {
        // If UI is not disconnected and parent is viewable, go up and expand
        if (mModelForItemFunctions && pParent->isViewable()) {
            // Now expand parents as needed
            Item *parentToExpand = pParent;
            while (parentToExpand) {
                if (parentToExpand == mRootItem) {
                    break; // no need to set it expanded
                }
                // parentToExpand is surely viewable (because this item is)
                if (parentToExpand->initialExpandStatus() == Item::ExpandExecuted) {
                    break;
                }

                expandViewNoninteractive(q->index(parentToExpand, 0));

                parentToExpand->setInitialExpandStatus(Item::ExpandExecuted);
                parentToExpand = parentToExpand->parent();
            }
        } else {
            // It isn't viewable or UI is disconnected: climb up marking only
            Item *parentToExpand = pParent->parent();
            while (parentToExpand) {
                if (parentToExpand == mRootItem) {
                    break; // no need to set it expanded
                }
                parentToExpand->setInitialExpandStatus(Item::ExpandNeeded);
                parentToExpand = parentToExpand->parent();
            }
        }
    }

    if (mi->isViewable()) {
        // mi is now viewable

        // sync subtree expanded status
        if (childNeedsExpanding) {
            if (mi->childItemCount() > 0) {
                if (mModelForItemFunctions) { // the UI is not disconnected
                    syncExpandedStateOfSubtree(mi); // sync the real state in the view
                }
            }
        }

        // apply the filter, if needed
        if (mFilter) {
            Q_ASSERT(mModelForItemFunctions); // the UI must be NOT disconnected here

            // apply the filter to subtree
            if (applyFilterToSubtree(mi, q->index(pParent, 0))) {
                // mi matched, expand parents (unconditionally)
                mView->ensureDisplayedWithParentsExpanded(mi);
            }
        }
    }

    // Now we need to propagate the property changes the upper levels.

    // If we have just inserted a message inside the root then no work needs to be done:
    // no grouping is in effect and the message is already in the right place.
    if (pParent == mRootItem) {
        return;
    }

    // If we have just removed the item from this parent and re-inserted it
    // then this operation was a simple re-sort. The code above didn't update
    // the properties when removing the item so we don't actually need
    // to make the updates back.
    if (oldParentWasTheSame) {
        return;
    }

    // FIXME: OPTIMIZE THIS: First propagate changes THEN syncExpandedStateOfSubtree()
    //        and applyFilterToSubtree... (needs some thinking though).

    // Time to propagate up.
    propagateItemPropertiesToParent(mi);

    // Aaah.. we're done. Time for a thea ? :)
}

// FIXME: ThreadItem ?
//
// Foo Bar, Joe Thommason, Martin Rox ... Eddie Maiden                    <date of the thread>
// Title                                      <number of messages>, Last by xxx <inner status>
//
// When messages are added, mark it as dirty only (?)

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternalForJobPass5(ViewItemJob *job, QElapsedTimer elapsedTimer)
{
    // In this pass we scan the group headers that are in mGroupHeadersThatNeedUpdate.
    // Empty groups get deleted while the other ones are re-sorted.

    int curIndex = job->currentIndex();

    auto it = mGroupHeadersThatNeedUpdate.begin();
    auto end = mGroupHeadersThatNeedUpdate.end();

    while (it != end) {
        if ((*it)->childItemCount() == 0) {
            // group with no children, kill it
            (*it)->parent()->takeChildItem(mModelForItemFunctions, *it);
            mGroupHeaderItemHash.remove((*it)->label());

            // If we were going to restore its position after the job step, well.. we can't do it anymore.
            if (mCurrentItemToRestoreAfterViewItemJobStep == (*it)) {
                mCurrentItemToRestoreAfterViewItemJobStep = nullptr;
            }

            // bye bye
            delete *it;
        } else {
            // Group with children: probably needs re-sorting.

            // Re-sorting here is an expensive operation.
            // In fact groups have been put in the QHash above on the assumption
            // that re-sorting *might* be needed but no real (expensive) check
            // has been done yet. Also by sorting a single group we might actually
            // put the others in the right place.
            // So finally check if re-sorting is *really* needed.
            bool needsReSorting;

            // A macro really improves readability here.
#define CHECK_IF_GROUP_NEEDS_RESORTING(_ItemDateComparator)                                                                                                    \
    switch (mSortOrder->groupSortDirection()) {                                                                                                                \
    case SortOrder::Ascending:                                                                                                                                 \
        needsReSorting = (*it)->parent()->d_ptr->childItemNeedsReSorting<_ItemDateComparator, true>(*it);                                                      \
        break;                                                                                                                                                 \
    case SortOrder::Descending:                                                                                                                                \
        needsReSorting = (*it)->parent()->d_ptr->childItemNeedsReSorting<_ItemDateComparator, false>(*it);                                                     \
        break;                                                                                                                                                 \
    default: /* should never happen */                                                                                                                         \
        needsReSorting = false;                                                                                                                                \
        break;                                                                                                                                                 \
    }

            switch (mSortOrder->groupSorting()) {
            case SortOrder::SortGroupsByDateTime:
                CHECK_IF_GROUP_NEEDS_RESORTING(ItemDateComparator)
                break;
            case SortOrder::SortGroupsByDateTimeOfMostRecent:
                CHECK_IF_GROUP_NEEDS_RESORTING(ItemMaxDateComparator)
                break;
            case SortOrder::SortGroupsBySenderOrReceiver:
                CHECK_IF_GROUP_NEEDS_RESORTING(ItemSenderOrReceiverComparator)
                break;
            case SortOrder::SortGroupsBySender:
                CHECK_IF_GROUP_NEEDS_RESORTING(ItemSenderComparator)
                break;
            case SortOrder::SortGroupsByReceiver:
                CHECK_IF_GROUP_NEEDS_RESORTING(ItemReceiverComparator)
                break;
            case SortOrder::NoGroupSorting:
                needsReSorting = false;
                break;
            default:
                // Should never happen... just assume re-sorting is not needed
                needsReSorting = false;
                break;
            }

            if (needsReSorting) {
                attachGroup(*it); // it will first detach and then re-attach in the proper place
            }
        }

        it = mGroupHeadersThatNeedUpdate.erase(it);

        curIndex++;

        // FIXME: In fact a single update is likely to manipulate
        //        a subtree with a LOT of messages inside. If interactivity is favored
        //        we should check the time really more often.
        if ((curIndex % mViewItemJobStepMessageCheckCount) == 0) {
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                if (it != mGroupHeadersThatNeedUpdate.end()) {
                    job->setCurrentIndex(curIndex);
                    return ViewItemJobInterrupted;
                }
            }
        }
    }

    return ViewItemJobCompleted;
}

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternalForJobPass4(ViewItemJob *job, QElapsedTimer elapsedTimer)
{
    // In this pass we scan mUnassignedMessageListForPass4 which now
    // contains both items with parents and items without parents.
    // We scan mUnassignedMessageList for messages without parent (the ones that haven't been
    // attached to the viewable tree yet) and find a suitable group for them. Then we simply
    // clear mUnassignedMessageList.

    // We call this pass "Grouping"

    int curIndex = job->currentIndex();
    int endIndex = job->endIndex();

    while (curIndex <= endIndex) {
        MessageItem *mi = mUnassignedMessageListForPass4[curIndex];
        if (!mi->parent()) {
            // Unassigned item: thread leader, insert into the proper group.
            // Locate the group (or root if no grouping requested)
            attachMessageToGroupHeader(mi);
        } else {
            // A parent was already assigned in Pass3: we have nothing to do here
        }
        curIndex++;

        // FIXME: In fact a single call to attachMessageToGroupHeader() is likely to manipulate
        //        a subtree with a LOT of messages inside. If interactivity is favored
        //        we should check the time really more often.
        if ((curIndex % mViewItemJobStepMessageCheckCount) == 0) {
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                if (curIndex <= endIndex) {
                    job->setCurrentIndex(curIndex);
                    return ViewItemJobInterrupted;
                }
            }
        }
    }

    mUnassignedMessageListForPass4.clear();
    return ViewItemJobCompleted;
}

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternalForJobPass3(ViewItemJob *job, QElapsedTimer elapsedTimer)
{
    // In this pass we scan the mUnassignedMessageListForPass3 and try to do construct the threads
    // by using subject based threading. If subject based threading is not in effect then
    // this pass turns to a nearly-no-op: at the end of Pass2 we have swapped the lists
    // and mUnassignedMessageListForPass3 is actually empty.

    // We don't shrink the mUnassignedMessageListForPass3 for two reasons:
    // - It would mess up this chunked algorithm by shifting indexes
    // - mUnassignedMessageList is a QList which is basically an array. It's faster
    //   to traverse an array of N entries than to remove K>0 entries one by one and
    //   to traverse the remaining N-K entries.

    int curIndex = job->currentIndex();
    int endIndex = job->endIndex();

    while (curIndex <= endIndex) {
        // If we're here, then threading is requested for sure.
        auto mi = mUnassignedMessageListForPass3[curIndex];
        if ((!mi->parent()) || (mi->threadingStatus() == MessageItem::ParentMissing)) {
            // Parent is missing (either "physically" with the item being not attached or "logically"
            // with the item being attached to a group or directly to the root.
            if (mi->subjectIsPrefixed()) {
                // We can try to guess it
                auto mparent = guessMessageParent(mi);

                if (mparent) {
                    // imperfect parent found
                    if (mi->isViewable()) {
                        // mi was already viewable, we're just trying to re-parent it better...
                        attachMessageToParent(mparent, mi);
                        if (!mparent->isViewable()) {
                            // re-attach it immediately (so current item is not lost)
                            auto topmost = mparent->topmostMessage();
                            Q_ASSERT(!topmost->parent()); // groups are always viewable!
                            topmost->setThreadingStatus(MessageItem::ParentMissing);
                            attachMessageToGroupHeader(topmost);
                        }
                    } else {
                        // mi wasn't viewable yet.. no need to attach parent
                        attachMessageToParent(mparent, mi);
                    }
                    // and we're done for now
                } else {
                    // so parent not found, (threadingStatus() is either MessageItem::ParentMissing or MessageItem::NonThreadable)
                    Q_ASSERT((mi->threadingStatus() == MessageItem::ParentMissing) || (mi->threadingStatus() == MessageItem::NonThreadable));
                    mUnassignedMessageListForPass4.append(mi); // this is ~O(1)
                    // and wait for Pass4
                }
            } else {
                // can't guess the parent as the subject isn't prefixed
                Q_ASSERT((mi->threadingStatus() == MessageItem::ParentMissing) || (mi->threadingStatus() == MessageItem::NonThreadable));
                mUnassignedMessageListForPass4.append(mi); // this is ~O(1)
                // and wait for Pass4
            }
        } else {
            // Has a parent: either perfect parent already found or non threadable.
            // Since we don't end here if mi has status of parent missing then mi must not have imperfect parent.
            Q_ASSERT(mi->threadingStatus() != MessageItem::ImperfectParentFound);
            Q_ASSERT(mi->isViewable());
        }

        curIndex++;

        // FIXME: In fact a single call to attachMessageToGroupHeader() is likely to manipulate
        //        a subtree with a LOT of messages inside. If interactivity is favored
        //        we should check the time really more often.
        if ((curIndex % mViewItemJobStepMessageCheckCount) == 0) {
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                if (curIndex <= endIndex) {
                    job->setCurrentIndex(curIndex);
                    return ViewItemJobInterrupted;
                }
            }
        }
    }

    mUnassignedMessageListForPass3.clear();
    return ViewItemJobCompleted;
}

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternalForJobPass2(ViewItemJob *job, QElapsedTimer elapsedTimer)
{
    // In this pass we scan the mUnassignedMessageList and try to do construct the threads.
    // If some thread leader message got attached to the viewable tree in Pass1Fill then
    // we'll also attach all of its children too. The thread leaders we were unable
    // to attach in Pass1Fill and their children (which we find here) will make it to the small Pass3

    // We don't shrink the mUnassignedMessageList for two reasons:
    // - It would mess up this chunked algorithm by shifting indexes
    // - mUnassignedMessageList is a QList which is basically an array. It's faster
    //   to traverse an array of N entries than to remove K>0 entries one by one and
    //   to traverse the remaining N-K entries.

    // We call this pass "Threading"

    int curIndex = job->currentIndex();
    int endIndex = job->endIndex();

    while (curIndex <= endIndex) {
        // If we're here, then threading is requested for sure.
        auto mi = mUnassignedMessageListForPass2[curIndex];
        // The item may or may not have a parent.
        // If it has no parent or it has a temporary one (mi->parent() && mi->threadingStatus() == MessageItem::ParentMissing)
        // then we attempt to (re-)thread it. Otherwise we just do nothing (the job has already been done by the previous steps).
        if ((!mi->parent()) || (mi->threadingStatus() == MessageItem::ParentMissing)) {
            qint64 parentId;
            auto mparent = mThreadingCache.parentForItem(mi, parentId);
            if (mparent && !mparent->hasAncestor(mi)) {
                mi->setThreadingStatus(MessageItem::PerfectParentFound);
                attachMessageToParent(mparent, mi, SkipCacheUpdate);
            } else {
                if (parentId > 0) {
                    // In second pass we have all available Items in mThreadingCache already. If
                    // mThreadingCache.parentForItem() returns null, but returns valid parentId then
                    // the Item was removed from Akonadi and our threading cache is out-of-date.
                    mThreadingCache.expireParent(mi);
                    mparent = findMessageParent(mi);
                } else if (parentId < 0) {
                    mparent = findMessageParent(mi);
                } else {
                    // parentId = 0: this message is a thread leader so don't
                    // bother resolving parent, it will be moved directly to
                    // Pass4 in the code below
                }

                if (mparent) {
                    // parent found, either perfect or imperfect
                    if (mi->isViewable()) {
                        // mi was already viewable, we're just trying to re-parent it better...
                        attachMessageToParent(mparent, mi);
                        if (!mparent->isViewable()) {
                            // re-attach it immediately (so current item is not lost)
                            auto topmost = mparent->topmostMessage();
                            Q_ASSERT(!topmost->parent()); // groups are always viewable!
                            topmost->setThreadingStatus(MessageItem::ParentMissing);
                            attachMessageToGroupHeader(topmost);
                        }
                    } else {
                        // mi wasn't viewable yet.. no need to attach parent
                        attachMessageToParent(mparent, mi);
                    }
                    // and we're done for now
                } else {
                    // so parent not found, (threadingStatus() is either MessageItem::ParentMissing or MessageItem::NonThreadable)
                    switch (mi->threadingStatus()) {
                    case MessageItem::ParentMissing:
                        if (mAggregation->threading() == Aggregation::PerfectReferencesAndSubject) {
                            // parent missing but still can be found in Pass3
                            mUnassignedMessageListForPass3.append(mi); // this is ~O(1)
                        } else {
                            // We're not doing subject based threading: will never be threaded, go straight to Pass4
                            mUnassignedMessageListForPass4.append(mi); // this is ~O(1)
                        }
                        break;
                    case MessageItem::NonThreadable:
                        // will never be threaded, go straight to Pass4
                        mUnassignedMessageListForPass4.append(mi); // this is ~O(1)
                        break;
                    default:
                        // a bug for sure
                        qCWarning(MESSAGELIST_LOG) << "ERROR: Invalid message threading status returned by findMessageParent()!";
                        Q_ASSERT(false);
                        break;
                    }
                }
            }
        } else {
            // Has a parent: either perfect parent already found or non threadable.
            // Since we don't end here if mi has status of parent missing then mi must not have imperfect parent.
            Q_ASSERT(mi->threadingStatus() != MessageItem::ImperfectParentFound);
            if (!mi->isViewable()) {
                qCWarning(MESSAGELIST_LOG) << "Non viewable message " << mi << " subject " << mi->subject().toUtf8().data();
                Q_ASSERT(mi->isViewable());
            }
        }

        curIndex++;

        // FIXME: In fact a single call to attachMessageToGroupHeader() is likely to manipulate
        //        a subtree with a LOT of messages inside. If interactivity is favored
        //        we should check the time really more often.
        if ((curIndex % mViewItemJobStepMessageCheckCount) == 0) {
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                if (curIndex <= endIndex) {
                    job->setCurrentIndex(curIndex);
                    return ViewItemJobInterrupted;
                }
            }
        }
    }

    mUnassignedMessageListForPass2.clear();
    return ViewItemJobCompleted;
}

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternalForJobPass1Fill(ViewItemJob *job, QElapsedTimer elapsedTimer)
{
    // In this pass we scan the a contiguous region of the underlying storage (that is
    // assumed to be FLAT) and create the corresponding MessageItem objects.
    // The deal is to show items to the user as soon as possible so in this pass we
    // *TRY* to attach them to the viewable tree (which is rooted on mRootItem).
    // Messages we're unable to attach for some reason (mainly due to threading) get appended
    // to mUnassignedMessageList and wait for Pass2.

    // We call this pass "Processing"

    // Should we use the receiver or the sender field for sorting ?
    bool bUseReceiver = mStorageModelContainsOutboundMessages;

    // The begin storage index of our work
    int curIndex = job->currentIndex();
    // The end storage index of our work.
    int endIndex = job->endIndex();

    unsigned long msgToSelect = mPreSelectionMode == PreSelectLastSelected ? mStorageModel->preSelectedMessage() : 0;

    MessageItem *mi = nullptr;

    while (curIndex <= endIndex) {
        // Create the message item with no parent: we'll set it later
        if (!mi) {
            mi = new MessageItem();
        } else {
            // a MessageItem discarded by a previous iteration: reuse it.
            Q_ASSERT(mi->parent() == nullptr);
        }

        if (!mStorageModel->initializeMessageItem(mi, curIndex, bUseReceiver)) {
            // ugh
            qCWarning(MESSAGELIST_LOG) << "Fill of the MessageItem at storage row index " << curIndex << " failed";
            curIndex++;
            continue;
        }

        // If we're supposed to pre-select a specific message, check if it's this one.
        if (msgToSelect != 0 && msgToSelect == mi->uniqueId()) {
            // Found, it's this one.
            // But actually it's not viewable (so not selectable). We must wait
            // until the end of the job to be 100% sure. So here we just translate
            // the unique id to a MessageItem pointer and wait.
            mLastSelectedMessageInFolder = mi;
            msgToSelect = 0; // already found, don't bother checking anymore
        }

        // Update the newest/oldest message, since we might be supposed to select those later
        if (mi->date() != static_cast<uint>(-1)) {
            if (!mOldestItem || mOldestItem->date() > mi->date()) {
                mOldestItem = mi;
            }
            if (!mNewestItem || mNewestItem->date() < mi->date()) {
                mNewestItem = mi;
            }
        }

        // Ok.. it passed the initial checks: we will not be discarding it.
        // Make this message item an invariant index to the underlying model storage.
        mInvariantRowMapper->createModelInvariantIndex(curIndex, mi);

        // Attempt to do threading as soon as possible (to display items to the user)
        if (mAggregation->threading() != Aggregation::NoThreading) {
            // Threading is requested

            // Fetch the data needed for proper threading
            // Add the item to the threading caches

            switch (mAggregation->threading()) {
            case Aggregation::PerfectReferencesAndSubject:
                mStorageModel->fillMessageItemThreadingData(mi, curIndex, StorageModel::PerfectThreadingReferencesAndSubject);

                // We also need to build the subject/reference-based threading cache
                addMessageToReferencesBasedThreadingCache(mi);
                addMessageToSubjectBasedThreadingCache(mi);
                break;
            case Aggregation::PerfectAndReferences:
                mStorageModel->fillMessageItemThreadingData(mi, curIndex, StorageModel::PerfectThreadingPlusReferences);
                addMessageToReferencesBasedThreadingCache(mi);
                break;
            default:
                mStorageModel->fillMessageItemThreadingData(mi, curIndex, StorageModel::PerfectThreadingOnly);
                break;
            }

            // Perfect/References threading cache
            mThreadingCacheMessageIdMD5ToMessageItem.insert(mi->messageIdMD5(), mi);

            // Register the current item into the threading cache
            mThreadingCache.addItemToCache(mi);

            // First of all look into the persistent cache
            qint64 parentId;
            Item *pParent = mThreadingCache.parentForItem(mi, parentId);
            if (pParent) {
                // We already have the parent MessageItem. Attach current message
                // to it and mark it as perfect
                mi->setThreadingStatus(MessageItem::PerfectParentFound);
                attachMessageToParent(pParent, mi);
            } else if (parentId > 0) {
                // We don't have the parent MessageItem yet, but we do know the
                // parent: delay for pass 2 when we will have the parent MessageItem
                // for sure.
                mi->setThreadingStatus(MessageItem::ParentMissing);
                mUnassignedMessageListForPass2.append(mi);
            } else if (parentId == 0) {
                // Message is a thread leader, skip straight to Pass4
                mi->setThreadingStatus(MessageItem::NonThreadable);
                mUnassignedMessageListForPass4.append(mi);
            } else {
                // Check if this item is a perfect parent for some imperfectly threaded
                // message (that is actually attached to it, but not necessarily to the
                // viewable root). If it is, then remove the imperfect child from its
                // current parent rebuild the hierarchy on the fly.
                bool needsImmediateReAttach = false;

                if (!mThreadingCacheMessageInReplyToIdMD5ToMessageItem.isEmpty()) { // unlikely
                    const auto lImperfectlyThreaded = mThreadingCacheMessageInReplyToIdMD5ToMessageItem.values(mi->messageIdMD5());
                    for (const auto it : lImperfectlyThreaded) {
                        Q_ASSERT(it->parent());
                        Q_ASSERT(it->parent() != mi);

                        if (!((it->threadingStatus() == MessageItem::ImperfectParentFound) || (it->threadingStatus() == MessageItem::ParentMissing))) {
                            qCritical() << "Got message " << it << " with threading status" << it->threadingStatus();
                            Q_ASSERT_X(false, "ModelPrivate::viewItemJobStepInternalForJobPass1Fill", "Wrong threading status");
                        }

                        // If the item was already attached to the view then
                        // re-attach it immediately. This will avoid a message
                        // being displayed for a short while in the view and then
                        // disappear until a perfect parent isn't found.
                        if (it->isViewable()) {
                            needsImmediateReAttach = true;
                        }

                        it->setThreadingStatus(MessageItem::PerfectParentFound);
                        attachMessageToParent(mi, it);
                    }
                }

                // FIXME: Might look by "References" too, here... (?)

                // Attempt to do threading with anything we already have in caches until now
                // Note that this is likely to work since thread-parent messages tend
                // to come before thread-children messages in the folders (simply because of
                // date of arrival).

                // First of all try to find a "perfect parent", that is the message for that
                // we have the ID in the "In-Reply-To" field. This is actually done by using
                // MD5 caches of the message ids because of speed. Collisions are very unlikely.

                const auto md5 = mi->inReplyToIdMD5();
                if (!md5.isEmpty()) {
                    // Have an In-Reply-To field MD5.
                    // In well behaved mailing lists 70% of the threadable messages get a parent here :)
                    pParent = mThreadingCacheMessageIdMD5ToMessageItem.value(md5, nullptr);

                    if (pParent) { // very likely
                        // Take care of self-referencing (which is always possible)
                        // and circular In-Reply-To reference loops which are possible
                        // in case this item was found to be a perfect parent for some
                        // imperfectly threaded message just above.
                        if ((mi == pParent) // self referencing message
                            || ((mi->childItemCount() > 0) // mi already has children, this is fast to determine
                                && pParent->hasAncestor(mi) // pParent is in the mi's children tree
                                )) {
                            // Bad, bad message.. it has In-Reply-To equal to Message-Id
                            // or it's in a circular In-Reply-To reference loop.
                            // Will wait for Pass2 with References-Id only
                            qCWarning(MESSAGELIST_LOG) << "Circular In-Reply-To reference loop detected in the message tree";
                            mUnassignedMessageListForPass2.append(mi);
                        } else {
                            // wow, got a perfect parent for this message!
                            mi->setThreadingStatus(MessageItem::PerfectParentFound);
                            attachMessageToParent(pParent, mi);
                            // we're done with this message (also for Pass2)
                        }
                    } else {
                        // got no parent
                        // will have to wait Pass2
                        mUnassignedMessageListForPass2.append(mi);
                    }
                } else {
                    // No In-Reply-To header.

                    bool mightHaveOtherMeansForThreading;

                    switch (mAggregation->threading()) {
                    case Aggregation::PerfectReferencesAndSubject:
                        mightHaveOtherMeansForThreading = mi->subjectIsPrefixed() || !mi->referencesIdMD5().isEmpty();
                        break;
                    case Aggregation::PerfectAndReferences:
                        mightHaveOtherMeansForThreading = !mi->referencesIdMD5().isEmpty();
                        break;
                    case Aggregation::PerfectOnly:
                        mightHaveOtherMeansForThreading = false;
                        break;
                    default:
                        // BUG: there shouldn't be other values (NoThreading is excluded in an upper branch)
                        Q_ASSERT(false);
                        mightHaveOtherMeansForThreading = false; // make gcc happy
                        break;
                    }

                    if (mightHaveOtherMeansForThreading) {
                        // We might have other means for threading this message, wait until Pass2
                        mUnassignedMessageListForPass2.append(mi);
                    } else {
                        // No other means for threading this message. This is either
                        // a standalone message or a thread leader.
                        // If there is no grouping in effect or thread leaders are just the "topmost"
                        // messages then we might be done with this one.
                        if ((mAggregation->grouping() == Aggregation::NoGrouping) || (mAggregation->threadLeader() == Aggregation::TopmostMessage)) {
                            // We're done with this message: it will be surely either toplevel (no grouping in effect)
                            // or a thread leader with a well defined group. Do it :)
                            // qCDebug(MESSAGELIST_LOG) << "Setting message status from " << mi->threadingStatus() << " to non threadable (1) " << mi;
                            mi->setThreadingStatus(MessageItem::NonThreadable);
                            // Locate the parent group for this item
                            attachMessageToGroupHeader(mi);
                            // we're done with this message (also for Pass2)
                        } else {
                            // Threads belong to the most recent message in the thread. This means
                            // that we have to wait until Pass2 or Pass3 to assign a group.
                            mUnassignedMessageListForPass2.append(mi);
                        }
                    }
                }

                if (needsImmediateReAttach && !mi->isViewable()) {
                    // The item gathered previously viewable children. They must be immediately
                    // re-shown. So this item must currently be attached to the view.
                    // This is a temporary measure: it will be probably still moved.
                    MessageItem *topmost = mi->topmostMessage();
                    Q_ASSERT(topmost->threadingStatus() == MessageItem::ParentMissing);
                    attachMessageToGroupHeader(topmost);
                }
            }
        } else {
            // else no threading requested: we don't even need Pass2
            // set not threadable status (even if it might be not true, but in this mode we don't care)
            // qCDebug(MESSAGELIST_LOG) << "Setting message status from " << mi->threadingStatus() << " to non threadable (2) " << mi;
            mi->setThreadingStatus(MessageItem::NonThreadable);
            // locate the parent group for this item
            if (mAggregation->grouping() == Aggregation::NoGrouping) {
                attachMessageToParent(mRootItem, mi); // no groups requested, attach directly to root
            } else {
                attachMessageToGroupHeader(mi);
            }
            // we're done with this message (also for Pass2)
        }

        mi = nullptr; // this item was pushed somewhere, create a new one at next iteration
        curIndex++;

        if ((curIndex % mViewItemJobStepMessageCheckCount) == 0) {
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                if (curIndex <= endIndex) {
                    job->setCurrentIndex(curIndex);
                    return ViewItemJobInterrupted;
                }
            }
        }
    }

    if (mi) {
        delete mi;
    }
    return ViewItemJobCompleted;
}

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternalForJobPass1Cleanup(ViewItemJob *job, QElapsedTimer elapsedTimer)
{
    Q_ASSERT(mModelForItemFunctions); // UI must be not disconnected here
    // In this pass we remove the MessageItem objects that are present in the job
    // and put their children in the unassigned message list.

    // Note that this list in fact contains MessageItem objects (we need dynamic_cast<>).
    QList<ModelInvariantIndex *> *invalidatedMessages = job->invariantIndexList();

    // We don't shrink the invalidatedMessages because it's basically an array.
    // It's faster to traverse an array of N entries than to remove K>0 entries
    // one by one and to traverse the remaining N-K entries.

    // The begin index of our work
    int curIndex = job->currentIndex();
    // The end index of our work.
    int endIndex = job->endIndex();

    if (curIndex == job->startIndex()) {
        Q_ASSERT(mOrphanChildrenHash.isEmpty());
    }

    while (curIndex <= endIndex) {
        // Get the underlying storage message data...
        auto dyingMessage = dynamic_cast<MessageItem *>(invalidatedMessages->at(curIndex));
        // This MUST NOT be null (otherwise we have a bug somewhere in this file).
        Q_ASSERT(dyingMessage);

        // If we were going to pre-select this message but we were interrupted
        // *before* it was actually made viewable, we just clear the pre-selection pointer
        // and unique id (abort pre-selection).
        if (dyingMessage == mLastSelectedMessageInFolder) {
            mLastSelectedMessageInFolder = nullptr;
            mPreSelectionMode = PreSelectNone;
        }

        // remove the message from any pending user job
        if (mPersistentSetManager) {
            mPersistentSetManager->removeMessageItemFromAllSets(dyingMessage);
            if (mPersistentSetManager->setCount() < 1) {
                delete mPersistentSetManager;
                mPersistentSetManager = nullptr;
            }
        }

        // Remove the message from threading cache before we start moving up the
        // children, so that they don't get mislead by the cache
        mThreadingCache.expireParent(dyingMessage);

        if (dyingMessage->parent()) {
            // Handle saving the current selection: if this item was the current before the step
            // then zero it out. We have killed it and it's OK for the current item to change.

            if (dyingMessage == mCurrentItemToRestoreAfterViewItemJobStep) {
                Q_ASSERT(dyingMessage->isViewable());
                // Try to select the item below the removed one as it helps in doing a "readon" of emails:
                // you read a message, decide to delete it and then go to the next.
                // Qt tends to select the message above the removed one instead (this is a hardcoded logic in
                // QItemSelectionModelPrivate::_q_rowsAboutToBeRemoved()).
                mCurrentItemToRestoreAfterViewItemJobStep = mView->messageItemAfter(dyingMessage, MessageTypeAny, false);

                if (!mCurrentItemToRestoreAfterViewItemJobStep) {
                    // There is no item below. Try the item above.
                    // We still do it better than qt which tends to find the *thread* above
                    // instead of the item above.
                    mCurrentItemToRestoreAfterViewItemJobStep = mView->messageItemBefore(dyingMessage, MessageTypeAny, false);
                }

                Q_ASSERT((!mCurrentItemToRestoreAfterViewItemJobStep) || mCurrentItemToRestoreAfterViewItemJobStep->isViewable());
            }

            if (dyingMessage->isViewable() && ((dyingMessage)->childItemCount() > 0) // has children
                && mView->isExpanded(q->index(dyingMessage, 0)) // is actually expanded
            ) {
                saveExpandedStateOfSubtree(dyingMessage);
            }

            auto oldParent = dyingMessage->parent();
            oldParent->takeChildItem(q, dyingMessage);

            // FIXME: This can generate many message movements.. it would be nicer
            //        to start from messages that are higher in the hierarchy so
            //        we would need to move less stuff above.

            if (oldParent != mRootItem) {
                messageDetachedUpdateParentProperties(oldParent, dyingMessage);
            }

            // We might have already removed its parent from the view, so it
            // might already be in the orphan child hash...
            if (dyingMessage->threadingStatus() == MessageItem::ParentMissing) {
                mOrphanChildrenHash.remove(dyingMessage); // this can turn to a no-op (dyingMessage not present in fact)
            }
        } else {
            // The dying message had no parent: this should happen only if it's already an orphan

            Q_ASSERT(dyingMessage->threadingStatus() == MessageItem::ParentMissing);
            Q_ASSERT(mOrphanChildrenHash.contains(dyingMessage));
            Q_ASSERT(dyingMessage != mCurrentItemToRestoreAfterViewItemJobStep);

            mOrphanChildrenHash.remove(dyingMessage);
        }

        if (mAggregation->threading() != Aggregation::NoThreading) {
            // Threading is requested: remove the message from threading caches.

            // Remove from the cache of potential parent items
            mThreadingCacheMessageIdMD5ToMessageItem.remove(dyingMessage->messageIdMD5());

            // If we also have a cache for subject/reference-based threading then remove the message from there too
            if (mAggregation->threading() == Aggregation::PerfectReferencesAndSubject) {
                removeMessageFromReferencesBasedThreadingCache(dyingMessage);
                removeMessageFromSubjectBasedThreadingCache(dyingMessage);
            } else if (mAggregation->threading() == Aggregation::PerfectAndReferences) {
                removeMessageFromReferencesBasedThreadingCache(dyingMessage);
            }

            // If this message wasn't perfectly parented then it might still be in another cache.
            switch (dyingMessage->threadingStatus()) {
            case MessageItem::ImperfectParentFound:
            case MessageItem::ParentMissing:
                if (!dyingMessage->inReplyToIdMD5().isEmpty()) {
                    mThreadingCacheMessageInReplyToIdMD5ToMessageItem.remove(dyingMessage->inReplyToIdMD5());
                }
                break;
            default:
                Q_ASSERT(!mThreadingCacheMessageInReplyToIdMD5ToMessageItem.contains(dyingMessage->inReplyToIdMD5(), dyingMessage));
                // make gcc happy
                break;
            }
        }

        while (auto childItem = dyingMessage->firstChildItem()) {
            auto childMessage = dynamic_cast<MessageItem *>(childItem);
            Q_ASSERT(childMessage);

            dyingMessage->takeChildItem(q, childMessage);

            if (mAggregation->threading() != Aggregation::NoThreading) {
                if (childMessage->threadingStatus() == MessageItem::PerfectParentFound) {
                    // If the child message was perfectly parented then now it had
                    // lost its perfect parent. Add to the cache of imperfectly parented.
                    if (!childMessage->inReplyToIdMD5().isEmpty()) {
                        Q_ASSERT(!mThreadingCacheMessageInReplyToIdMD5ToMessageItem.contains(childMessage->inReplyToIdMD5(), childMessage));
                        mThreadingCacheMessageInReplyToIdMD5ToMessageItem.insert(childMessage->inReplyToIdMD5(), childMessage);
                    }
                }
            }

            // Parent is gone
            childMessage->setThreadingStatus(MessageItem::ParentMissing);

            // If the child (or any message in its subtree) is going to be selected,
            // then we must immediately reattach it to a temporary group in order for the
            // selection to be preserved across multiple steps. Otherwise we could end
            // with the child-to-be-selected being non viewable at the end
            // of the view job step. Attach to a temporary group.
            if (
                // child is going to be re-selected
                (childMessage == mCurrentItemToRestoreAfterViewItemJobStep)
                || (
                    // there is a message that is going to be re-selected
                    mCurrentItemToRestoreAfterViewItemJobStep && // that message is in the childMessage subtree
                    mCurrentItemToRestoreAfterViewItemJobStep->hasAncestor(childMessage))) {
                attachMessageToGroupHeader(childMessage);

                Q_ASSERT(childMessage->isViewable());
            }

            mOrphanChildrenHash.insert(childMessage, childMessage);
        }

        if (mNewestItem == dyingMessage) {
            mNewestItem = nullptr;
        }
        if (mOldestItem == dyingMessage) {
            mOldestItem = nullptr;
        }

        delete dyingMessage;

        curIndex++;

        // FIXME: Maybe we should check smaller steps here since the
        //        code above can generate large message tree movements
        //        for each single item we sweep in the invalidatedMessages list.
        if ((curIndex % mViewItemJobStepMessageCheckCount) == 0) {
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                if (curIndex <= endIndex) {
                    job->setCurrentIndex(curIndex);
                    return ViewItemJobInterrupted;
                }
            }
        }
    }

    // We looped over the entire deleted message list.

    job->setCurrentIndex(endIndex + 1);

    // A quick last cleaning pass: this is usually very fast so we don't have a real
    // Pass enumeration for it. We just include it as trailer of Pass1Cleanup to be executed
    // when job->currentIndex() > job->endIndex();

    // We move all the messages from the orphan child hash to the unassigned message
    // list and get them ready for the standard Pass2.

    auto it = mOrphanChildrenHash.begin();
    auto end = mOrphanChildrenHash.end();

    curIndex = 0;

    while (it != end) {
        mUnassignedMessageListForPass2.append(*it);

        it = mOrphanChildrenHash.erase(it);

        // This is still interruptible

        curIndex++;

        // FIXME: We could take "larger" steps here
        if ((curIndex % mViewItemJobStepMessageCheckCount) == 0) {
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                if (it != mOrphanChildrenHash.end()) {
                    return ViewItemJobInterrupted;
                }
            }
        }
    }

    return ViewItemJobCompleted;
}

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternalForJobPass1Update(ViewItemJob *job, QElapsedTimer elapsedTimer)
{
    Q_ASSERT(mModelForItemFunctions); // UI must be not disconnected here

    // In this pass we simply update the MessageItem objects that are present in the job.

    // Note that this list in fact contains MessageItem objects (we need dynamic_cast<>).
    auto messagesThatNeedUpdate = job->invariantIndexList();

    // We don't shrink the messagesThatNeedUpdate because it's basically an array.
    // It's faster to traverse an array of N entries than to remove K>0 entries
    // one by one and to traverse the remaining N-K entries.

    // The begin index of our work
    int curIndex = job->currentIndex();
    // The end index of our work.
    int endIndex = job->endIndex();

    while (curIndex <= endIndex) {
        // Get the underlying storage message data...
        auto message = dynamic_cast<MessageItem *>(messagesThatNeedUpdate->at(curIndex));
        // This MUST NOT be null (otherwise we have a bug somewhere in this file).
        Q_ASSERT(message);

        int row = mInvariantRowMapper->modelInvariantIndexToModelIndexRow(message);

        if (row < 0) {
            // Must have been invalidated (so it's basically about to be deleted)
            Q_ASSERT(!message->isValid());
            // Skip it here.
            curIndex++;
            continue;
        }

        time_t prevDate = message->date();
        time_t prevMaxDate = message->maxDate();
        bool toDoStatus = message->status().isToAct();
        bool prevUnreadStatus = !message->status().isRead();
        bool prevImportantStatus = message->status().isImportant();

        // The subject/reference based threading cache is sorted by date: we must remove
        // the item and re-insert it since updateMessageItemData() may change the date too.
        if (mAggregation->threading() == Aggregation::PerfectReferencesAndSubject) {
            removeMessageFromReferencesBasedThreadingCache(message);
            removeMessageFromSubjectBasedThreadingCache(message);
        } else if (mAggregation->threading() == Aggregation::PerfectAndReferences) {
            removeMessageFromReferencesBasedThreadingCache(message);
        }

        // Do update
        mStorageModel->updateMessageItemData(message, row);
        const QModelIndex idx = q->index(message, 0);
        Q_EMIT q->dataChanged(idx, idx);

        // Reinsert the item to the cache, if needed
        if (mAggregation->threading() == Aggregation::PerfectReferencesAndSubject) {
            addMessageToReferencesBasedThreadingCache(message);
            addMessageToSubjectBasedThreadingCache(message);
        } else if (mAggregation->threading() == Aggregation::PerfectAndReferences) {
            addMessageToReferencesBasedThreadingCache(message);
        }

        int propertyChangeMask = 0;

        if (prevDate != message->date()) {
            propertyChangeMask |= DateChanged;
        }
        if (prevMaxDate != message->maxDate()) {
            propertyChangeMask |= MaxDateChanged;
        }
        if (toDoStatus != message->status().isToAct()) {
            propertyChangeMask |= ActionItemStatusChanged;
        }
        if (prevUnreadStatus != (!message->status().isRead())) {
            propertyChangeMask |= UnreadStatusChanged;
        }
        if (prevImportantStatus != (!message->status().isImportant())) {
            propertyChangeMask |= ImportantStatusChanged;
        }

        if (propertyChangeMask) {
            // Some message data has changed
            // now we need to handle the changes that might cause re-grouping/re-sorting
            // and propagate them to the parents.

            Item *pParent = message->parent();

            if (pParent && (pParent != mRootItem)) {
                // The following function will return true if itemParent may be affected by the change.
                // If the itemParent isn't affected, we stop climbing.
                if (handleItemPropertyChanges(propertyChangeMask, pParent, message)) {
                    Q_ASSERT(message->parent()); // handleItemPropertyChanges() must never leave an item detached

                    // Note that actually message->parent() may be different than pParent since
                    // handleItemPropertyChanges() may have re-grouped it.

                    // Time to propagate up.
                    propagateItemPropertiesToParent(message);
                }
            } // else there is no parent so the item isn't attached to the view: re-grouping/re-sorting not needed.
        } // else message data didn't change an there is nothing interesting to do

        // (re-)apply the filter, if needed
        if (mFilter && message->isViewable()) {
            // In all the other cases we (re-)apply the filter to the topmost subtree that this message is in.
            Item *pTopMostNonRoot = message->topmostNonRoot();

            Q_ASSERT(pTopMostNonRoot);
            Q_ASSERT(pTopMostNonRoot != mRootItem);
            Q_ASSERT(pTopMostNonRoot->parent() == mRootItem);

            // FIXME: The call below works, but it's expensive when we are updating
            //        a lot of items with filtering enabled. This is because the updated
            //        items are likely to be in the same subtree which we then filter multiple times.
            //        A point for us is that when filtering there shouldn't be really many
            //        items in the view so the user isn't going to update a lot of them at once...
            //        Well... anyway, the alternative would be to write yet another
            //        specialized routine that would update only the "message" item
            //        above and climb up eventually hiding parents (without descending the sibling subtrees again).
            //        If people complain about performance in this particular case I'll consider that solution.

            applyFilterToSubtree(pTopMostNonRoot, QModelIndex());
        } // otherwise there is no filter or the item isn't viewable: very likely
          // left detached while propagating property changes. Will filter it
          // on reattach.

        // Done updating this message

        curIndex++;

        // FIXME: Maybe we should check smaller steps here since the
        //        code above can generate large message tree movements
        //        for each single item we sweep in the messagesThatNeedUpdate list.
        if ((curIndex % mViewItemJobStepMessageCheckCount) == 0) {
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                if (curIndex <= endIndex) {
                    job->setCurrentIndex(curIndex);
                    return ViewItemJobInterrupted;
                }
            }
        }
    }

    return ViewItemJobCompleted;
}

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternalForJob(ViewItemJob *job, QElapsedTimer elapsedTimer)
{
    // This function does a timed chunk of work for a single Fill View job.
    // It attempts to process messages until a timeout forces it to return to the caller.

    // A macro would improve readability here but since this is a good point
    // to place debugger breakpoints then we need it explicitly.
    // A (template) helper would need to pass many parameters and would not be inlined...

    if (job->currentPass() == ViewItemJob::Pass1Fill) {
        // We're in Pass1Fill of the job.
        switch (viewItemJobStepInternalForJobPass1Fill(job, elapsedTimer)) {
        case ViewItemJobInterrupted:
            // current job interrupted by timeout: propagate status to caller
            return ViewItemJobInterrupted;
            break;
        case ViewItemJobCompleted:
            // pass 1 has been completed
            // # TODO: Refactor this, make it virtual or whatever, but switch == bad, code duplication etc
            job->setCurrentPass(ViewItemJob::Pass2);
            job->setStartIndex(0);
            job->setEndIndex(mUnassignedMessageListForPass2.count() - 1);
            // take care of small jobs which never timeout by themselves because
            // of a small number of messages. At the end of each job check
            // the time used and if we're timeoutting and there is another job
            // then interrupt.
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                return ViewItemJobInterrupted;
            } // else proceed with the next pass
            break;
        default:
            // This is *really* a BUG
            qCWarning(MESSAGELIST_LOG) << "ERROR: returned an invalid result";
            Q_ASSERT(false);
            break;
        }
    } else if (job->currentPass() == ViewItemJob::Pass1Cleanup) {
        // We're in Pass1Cleanup of the job.
        switch (viewItemJobStepInternalForJobPass1Cleanup(job, elapsedTimer)) {
        case ViewItemJobInterrupted:
            // current job interrupted by timeout: propagate status to caller
            return ViewItemJobInterrupted;
            break;
        case ViewItemJobCompleted:
            // pass 1 has been completed
            job->setCurrentPass(ViewItemJob::Pass2);
            job->setStartIndex(0);
            job->setEndIndex(mUnassignedMessageListForPass2.count() - 1);
            // take care of small jobs which never timeout by themselves because
            // of a small number of messages. At the end of each job check
            // the time used and if we're timeoutting and there is another job
            // then interrupt.
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                return ViewItemJobInterrupted;
            } // else proceed with the next pass
            break;
        default:
            // This is *really* a BUG
            qCWarning(MESSAGELIST_LOG) << "ERROR: returned an invalid result";
            Q_ASSERT(false);
            break;
        }
    } else if (job->currentPass() == ViewItemJob::Pass1Update) {
        // We're in Pass1Update of the job.
        switch (viewItemJobStepInternalForJobPass1Update(job, elapsedTimer)) {
        case ViewItemJobInterrupted:
            // current job interrupted by timeout: propagate status to caller
            return ViewItemJobInterrupted;
            break;
        case ViewItemJobCompleted:
            // pass 1 has been completed
            // Since Pass2, Pass3 and Pass4 are empty for an Update operation
            // we simply skip them. (TODO: Triple-verify this assertion...).
            job->setCurrentPass(ViewItemJob::Pass5);
            job->setStartIndex(0);
            job->setEndIndex(mGroupHeadersThatNeedUpdate.count() - 1);
            // take care of small jobs which never timeout by themselves because
            // of a small number of messages. At the end of each job check
            // the time used and if we're timeoutting and there is another job
            // then interrupt.
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                return ViewItemJobInterrupted;
            } // else proceed with the next pass
            break;
        default:
            // This is *really* a BUG
            qCWarning(MESSAGELIST_LOG) << "ERROR: returned an invalid result";
            Q_ASSERT(false);
            break;
        }
    }

    // Pass1Fill/Pass1Cleanup/Pass1Update has been already completed.

    if (job->currentPass() == ViewItemJob::Pass2) {
        // We're in Pass2 of the job.
        switch (viewItemJobStepInternalForJobPass2(job, elapsedTimer)) {
        case ViewItemJobInterrupted:
            // current job interrupted by timeout: propagate status to caller
            return ViewItemJobInterrupted;
            break;
        case ViewItemJobCompleted:
            // pass 2 has been completed
            job->setCurrentPass(ViewItemJob::Pass3);
            job->setStartIndex(0);
            job->setEndIndex(mUnassignedMessageListForPass3.count() - 1);
            // take care of small jobs which never timeout by themselves because
            // of a small number of messages. At the end of each job check
            // the time used and if we're timeoutting and there is another job
            // then interrupt.
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                return ViewItemJobInterrupted;
            }
            // else proceed with the next pass
            break;
        default:
            // This is *really* a BUG
            qCWarning(MESSAGELIST_LOG) << "ERROR: returned an invalid result";
            Q_ASSERT(false);
            break;
        }
    }

    if (job->currentPass() == ViewItemJob::Pass3) {
        // We're in Pass3 of the job.
        switch (viewItemJobStepInternalForJobPass3(job, elapsedTimer)) {
        case ViewItemJobInterrupted:
            // current job interrupted by timeout: propagate status to caller
            return ViewItemJobInterrupted;
        case ViewItemJobCompleted:
            // pass 3 has been completed
            job->setCurrentPass(ViewItemJob::Pass4);
            job->setStartIndex(0);
            job->setEndIndex(mUnassignedMessageListForPass4.count() - 1);
            // take care of small jobs which never timeout by themselves because
            // of a small number of messages. At the end of each job check
            // the time used and if we're timeoutting and there is another job
            // then interrupt.
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                return ViewItemJobInterrupted;
            }
            // else proceed with the next pass
            break;
        default:
            // This is *really* a BUG
            qCWarning(MESSAGELIST_LOG) << "ERROR: returned an invalid result";
            Q_ASSERT(false);
            break;
        }
    }

    if (job->currentPass() == ViewItemJob::Pass4) {
        // We're in Pass4 of the job.
        switch (viewItemJobStepInternalForJobPass4(job, elapsedTimer)) {
        case ViewItemJobInterrupted:
            // current job interrupted by timeout: propagate status to caller
            return ViewItemJobInterrupted;
        case ViewItemJobCompleted:
            // pass 4 has been completed
            job->setCurrentPass(ViewItemJob::Pass5);
            job->setStartIndex(0);
            job->setEndIndex(mGroupHeadersThatNeedUpdate.count() - 1);
            // take care of small jobs which never timeout by themselves because
            // of a small number of messages. At the end of each job check
            // the time used and if we're timeoutting and there is another job
            // then interrupt.
            if (elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) {
                return ViewItemJobInterrupted;
            }
            // else proceed with the next pass
            break;
        default:
            // This is *really* a BUG
            qCWarning(MESSAGELIST_LOG) << "ERROR: returned an invalid result";
            Q_ASSERT(false);
            break;
        }
    }

    // Pass4 has been already completed. Proceed to Pass5.
    return viewItemJobStepInternalForJobPass5(job, elapsedTimer);
}

#ifdef KDEPIM_FOLDEROPEN_PROFILE

// Namespace to collect all the vars and functions for KDEPIM_FOLDEROPEN_PROFILE
namespace Stats
{
// Number of existing jobs/passes
static const int numberOfPasses = ViewItemJob::LastIndex;

// The pass in the last call of viewItemJobStepInternal(), used to detect when
// a new pass starts
static int lastPass = -1;

// Total number of messages in the folder
static int totalMessages;

// Per-Job data
static int numElements[numberOfPasses];
static int totalTime[numberOfPasses];
static int chunks[numberOfPasses];

// Time, in msecs for some special operations
static int expandingTreeTime;
static int layoutChangeTime;

// Descriptions of the job, for nicer debug output
static const char *jobDescription[numberOfPasses] = {"Creating items from messages and simple threading",
                                                     "Removing messages",
                                                     "Updating messages",
                                                     "Additional Threading",
                                                     "Subject-Based threading",
                                                     "Grouping",
                                                     "Group resorting + cleanup"};

// Timer to track time between start of first job and end of last job
static QElapsedTimer firstStartTime;

// Timer to track time the current job takes
static QElapsedTimer currentJobStartTime;

// Zeros the stats, to be called when the first job starts
static void resetStats()
{
    totalMessages = 0;
    layoutChangeTime = 0;
    expandingTreeTime = 0;
    lastPass = -1;
    for (int i = 0; i < numberOfPasses; ++i) {
        numElements[i] = 0;
        totalTime[i] = 0;
        chunks[i] = 0;
    }
}
} // namespace Stats

void ModelPrivate::printStatistics()
{
    using namespace Stats;
    int totalTotalTime = 0;
    int completeTime = firstStartTime.elapsed();
    for (int i = 0; i < numberOfPasses; ++i) {
        totalTotalTime += totalTime[i];
    }

    float msgPerSecond = totalMessages / (totalTotalTime / 1000.0f);
    float msgPerSecondComplete = totalMessages / (completeTime / 1000.0f);

    int messagesWithSameSubjectAvg = 0;
    int messagesWithSameSubjectMax = 0;
    for (const auto messages : std::as_const(mThreadingCacheMessageSubjectMD5ToMessageItem)) {
        if (messages->size() > messagesWithSameSubjectMax) {
            messagesWithSameSubjectMax = messages->size();
        }
        messagesWithSameSubjectAvg += messages->size();
    }
    messagesWithSameSubjectAvg = messagesWithSameSubjectAvg / (float)mThreadingCacheMessageSubjectMD5ToMessageItem.size();

    int totalThreads = 0;
    if (!mGroupHeaderItemHash.isEmpty()) {
        for (const GroupHeaderItem *groupHeader : std::as_const(mGroupHeaderItemHash)) {
            totalThreads += groupHeader->childItemCount();
        }
    } else {
        totalThreads = mRootItem->childItemCount();
    }

    qCDebug(MESSAGELIST_LOG) << "Finished filling the view with" << totalMessages << "messages";
    qCDebug(MESSAGELIST_LOG) << "That took" << totalTotalTime << "msecs inside the model and" << completeTime << "in total.";
    qCDebug(MESSAGELIST_LOG) << (totalTotalTime / (float)completeTime) * 100.0f << "percent of the time was spent in the model.";
    qCDebug(MESSAGELIST_LOG) << "Time for layoutChanged(), in msecs:" << layoutChangeTime << "(" << (layoutChangeTime / (float)totalTotalTime) * 100.0f
                             << "percent )";
    qCDebug(MESSAGELIST_LOG) << "Time to expand tree, in msecs:" << expandingTreeTime << "(" << (expandingTreeTime / (float)totalTotalTime) * 100.0f
                             << "percent )";
    qCDebug(MESSAGELIST_LOG) << "Number of messages per second in the model:" << msgPerSecond;
    qCDebug(MESSAGELIST_LOG) << "Number of messages per second in total:" << msgPerSecondComplete;
    qCDebug(MESSAGELIST_LOG) << "Number of threads:" << totalThreads;
    qCDebug(MESSAGELIST_LOG) << "Number of groups:" << mGroupHeaderItemHash.size();
    qCDebug(MESSAGELIST_LOG) << "Messages per thread:" << totalMessages / (float)totalThreads;
    qCDebug(MESSAGELIST_LOG) << "Threads per group:" << totalThreads / (float)mGroupHeaderItemHash.size();
    qCDebug(MESSAGELIST_LOG) << "Messages with the same subject:"
                             << "Max:" << messagesWithSameSubjectMax << "Avg:" << messagesWithSameSubjectAvg;
    qCDebug(MESSAGELIST_LOG);
    qCDebug(MESSAGELIST_LOG) << "Now follows a breakdown of the jobs.";
    qCDebug(MESSAGELIST_LOG);
    for (int i = 0; i < numberOfPasses; ++i) {
        if (totalTime[i] == 0) {
            continue;
        }
        float elementsPerSecond = numElements[i] / (totalTime[i] / 1000.0f);
        float percent = totalTime[i] / (float)totalTotalTime * 100.0f;
        qCDebug(MESSAGELIST_LOG) << "----------------------------------------------";
        qCDebug(MESSAGELIST_LOG) << "Job" << i + 1 << "(" << jobDescription[i] << ")";
        qCDebug(MESSAGELIST_LOG) << "Share of complete time:" << percent << "percent";
        qCDebug(MESSAGELIST_LOG) << "Time in msecs:" << totalTime[i];
        qCDebug(MESSAGELIST_LOG) << "Number of elements:" << numElements[i]; // TODO: map of element string
        qCDebug(MESSAGELIST_LOG) << "Elements per second:" << elementsPerSecond;
        qCDebug(MESSAGELIST_LOG) << "Number of chunks:" << chunks[i];
        qCDebug(MESSAGELIST_LOG);
    }

    qCDebug(MESSAGELIST_LOG) << "==========================================================";
    resetStats();
}

#endif

ModelPrivate::ViewItemJobResult ModelPrivate::viewItemJobStepInternal()
{
    // This function does a timed chunk of work in our View Fill operation.
    // It attempts to do processing until it either runs out of jobs
    // to be done or a timeout forces it to interrupt and jump back to the caller.

    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    while (!mViewItemJobs.isEmpty()) {
        // Have a job to do.
        ViewItemJob *job = mViewItemJobs.constFirst();

#ifdef KDEPIM_FOLDEROPEN_PROFILE

        // Here we check if an old job has just completed or if we are at the start of the
        // first job. We then initialize job data stuff and timers based on this.

        const int currentPass = job->currentPass();
        const bool firstChunk = currentPass != Stats::lastPass;
        if (currentPass != Stats::lastPass && Stats::lastPass != -1) {
            Stats::totalTime[Stats::lastPass] = Stats::currentJobStartTime.elapsed();
        }
        const bool firstJob = job->currentPass() == ViewItemJob::Pass1Fill && firstChunk;
        const int elements = job->endIndex() - job->startIndex();
        if (firstJob) {
            Stats::resetStats();
            Stats::totalMessages = elements;
            Stats::firstStartTime.restart();
        }
        if (firstChunk) {
            Stats::numElements[currentPass] = elements;
            Stats::currentJobStartTime.restart();
        }
        Stats::chunks[currentPass]++;
        Stats::lastPass = currentPass;

#endif

        mViewItemJobStepIdleInterval = job->idleInterval();
        mViewItemJobStepChunkTimeout = job->chunkTimeout();
        mViewItemJobStepMessageCheckCount = job->messageCheckCount();

        if (job->disconnectUI()) {
            mModelForItemFunctions = nullptr; // disconnect the UI for this job
            Q_ASSERT(mLoading); // this must be true in the first job
            // FIXME: Should assert yet more that this is the very first job for this StorageModel
            //        Asserting only mLoading is not enough as we could be using a two-jobs loading strategy
            //        or this could be a job enqueued before the first job has completed.
        } else {
            // With a connected UI we need to avoid the view to update the scrollbars at EVERY insertion or expansion.
            // QTreeViewPrivate::updateScrollBars() is very expensive as it loops through ALL the items in the view every time.
            // We can't disable the function directly as it's hidden in the private data object of QTreeView
            // but we can disable the parent QTreeView::updateGeometries() instead.
            // We will trigger it "manually" at the end of the step.
            mView->ignoreUpdateGeometries(true);

            // Ok.. I know that this seems unbelieveable but disabling updates actually
            // causes a (significant) performance loss in most cases. This is probably because QTreeView
            // uses delayed layouts when updates are disabled which should be delayed but in
            // fact are "forced" by next item insertions. The delayed layout algorithm, then
            // is probably slower than the non-delayed one.
            // Disabling the paintEvent() doesn't seem to work either.
            // mView->setUpdatesEnabled( false );
        }

        switch (viewItemJobStepInternalForJob(job, elapsedTimer)) {
        case ViewItemJobInterrupted:
            // current job interrupted by timeout: will propagate status to caller
            // but before this, give some feedback to the user

            // FIXME: This is now inaccurate, think of something else
            switch (job->currentPass()) {
            case ViewItemJob::Pass1Fill:
            case ViewItemJob::Pass1Cleanup:
            case ViewItemJob::Pass1Update:
                Q_EMIT q->statusMessage(i18np("Processed 1 Message of %2",
                                              "Processed %1 Messages of %2",
                                              job->currentIndex() - job->startIndex(),
                                              job->endIndex() - job->startIndex() + 1));
                break;
            case ViewItemJob::Pass2:
                Q_EMIT q->statusMessage(i18np("Threaded 1 Message of %2",
                                              "Threaded %1 Messages of %2",
                                              job->currentIndex() - job->startIndex(),
                                              job->endIndex() - job->startIndex() + 1));
                break;
            case ViewItemJob::Pass3:
                Q_EMIT q->statusMessage(i18np("Threaded 1 Message of %2",
                                              "Threaded %1 Messages of %2",
                                              job->currentIndex() - job->startIndex(),
                                              job->endIndex() - job->startIndex() + 1));
                break;
            case ViewItemJob::Pass4:
                Q_EMIT q->statusMessage(i18np("Grouped 1 Thread of %2",
                                              "Grouped %1 Threads of %2",
                                              job->currentIndex() - job->startIndex(),
                                              job->endIndex() - job->startIndex() + 1));
                break;
            case ViewItemJob::Pass5:
                Q_EMIT q->statusMessage(i18np("Updated 1 Group of %2",
                                              "Updated %1 Groups of %2",
                                              job->currentIndex() - job->startIndex(),
                                              job->endIndex() - job->startIndex() + 1));
                break;
            default:
                break;
            }

            if (!job->disconnectUI()) {
                mView->ignoreUpdateGeometries(false);
                // explicit call to updateGeometries() here
                mView->updateGeometries();
            }

            return ViewItemJobInterrupted;
            break;
        case ViewItemJobCompleted:

            // If this job worked with a disconnected UI, Q_EMIT layoutChanged()
            // to reconnect it. We go back to normal operation now.
            if (job->disconnectUI()) {
                mModelForItemFunctions = q;
                // This call would destroy the expanded state of items.
                // This is why when mModelForItemFunctions was 0 we didn't actually expand them
                // but we just set a "ExpandNeeded" mark...
#ifdef KDEPIM_FOLDEROPEN_PROFILE
                QElapsedTimer layoutChangedTimer;
                layoutChangedTimer.start();
#endif
                mView->modelAboutToEmitLayoutChanged();
                Q_EMIT q->layoutChanged();
                mView->modelEmittedLayoutChanged();

#ifdef KDEPIM_FOLDEROPEN_PROFILE
                Stats::layoutChangeTime = layoutChangedTimer.elapsed();
                QElapsedTimer expandingTime;
                expandingTime.start();
#endif

                // expand all the items that need it in a single sweep

                // FIXME: This takes quite a lot of time, it could be made an interruptible job

                auto rootChildItems = mRootItem->childItems();
                if (rootChildItems) {
                    for (const auto it : std::as_const(*rootChildItems)) {
                        if (it->initialExpandStatus() == Item::ExpandNeeded) {
                            syncExpandedStateOfSubtree(it);
                        }
                    }
                }
#ifdef KDEPIM_FOLDEROPEN_PROFILE
                Stats::expandingTreeTime = expandingTime.elapsed();
#endif
            } else {
                mView->ignoreUpdateGeometries(false);
                // explicit call to updateGeometries() here
                mView->updateGeometries();
            }

            // this job has been completed
            delete mViewItemJobs.takeFirst();

#ifdef KDEPIM_FOLDEROPEN_PROFILE
            // Last job finished!
            Stats::totalTime[currentPass] = Stats::currentJobStartTime.elapsed();
            printStatistics();
#endif

            // take care of small jobs which never timeout by themselves because
            // of a small number of messages. At the end of each job check
            // the time used and if we're timeoutting and there is another job
            // then interrupt.
            if ((elapsedTimer.elapsed() > mViewItemJobStepChunkTimeout) || (elapsedTimer.elapsed() < 0)) {
                if (!mViewItemJobs.isEmpty()) {
                    return ViewItemJobInterrupted;
                }
                // else it's completed in fact
            } // else proceed with the next job

            break;
        default:
            // This is *really* a BUG
            qCWarning(MESSAGELIST_LOG) << "ERROR: returned an invalid result";
            Q_ASSERT(false);
            break;
        }
    }

    // no more jobs

    Q_EMIT q->statusMessage(i18nc("@info:status Finished view fill", "Ready"));

    return ViewItemJobCompleted;
}

void ModelPrivate::viewItemJobStep()
{
    // A single step in the View Fill operation.
    // This function wraps viewItemJobStepInternal() which does the step job
    // and either completes it or stops because of a timeout.
    // If the job is stopped then we start a zero-msecs timer to call us
    // back and resume the job. Otherwise we're just done.

    mViewItemJobStepStartTime = ::time(nullptr);

    if (mFillStepTimer.isActive()) {
        mFillStepTimer.stop();
    }

    if (!mStorageModel) {
        return; // nothing more to do
    }

    // Save the current item in the view as our process may
    // cause items to be reparented (and QTreeView will forget the current item in the meantime).
    // This machinery is also needed when we're about to remove items from the view in
    // a cleanup job: we'll be trying to set as current the item after the one removed.

    QModelIndex currentIndexBeforeStep = mView->currentIndex();
    Item *currentItemBeforeStep = currentIndexBeforeStep.isValid() ? static_cast<Item *>(currentIndexBeforeStep.internalPointer()) : nullptr;

    // mCurrentItemToRestoreAfterViewItemJobStep will be zeroed out if it's killed
    mCurrentItemToRestoreAfterViewItemJobStep = currentItemBeforeStep;

    // Save the current item position in the viewport as QTreeView fails to keep
    // the current item in the sample place when items are added or removed...
    QRect rectBeforeViewItemJobStep;

    const int lockViewDirection = mView->scrollingLockDirection();

    // This is generally SLOW AS HELL... (so we avoid it if we lock the view and thus don't need it)
    if (mCurrentItemToRestoreAfterViewItemJobStep && (lockViewDirection == 0)) {
        rectBeforeViewItemJobStep = mView->visualRect(currentIndexBeforeStep);
    }

    // FIXME: If the current item is NOT in the view, preserve the position
    //        of the top visible item. This will make the view move yet less.

    // Insulate the View from (very likely spurious) "currentChanged()" signals.
    mView->ignoreCurrentChanges(true);

    // And go to real work.
    switch (viewItemJobStepInternal()) {
    case ViewItemJobInterrupted:
        // Operation timed out, need to resume in a while
        if (!mInLengthyJobBatch) {
            mInLengthyJobBatch = true;
        }
        mFillStepTimer.start(mViewItemJobStepIdleInterval); // this is a single shot timer connected to viewItemJobStep()
        // and go dealing with current/selection out of the switch.
        break;
    case ViewItemJobCompleted:
        // done :)

        Q_ASSERT(mModelForItemFunctions); // UI must be no (longer) disconnected in this state

        // Ask the view to remove the eventual busy indications
        if (mInLengthyJobBatch) {
            mInLengthyJobBatch = false;
        }

        if (mLoading) {
            mLoading = false;
            mView->modelFinishedLoading();
            slotApplyFilter();
        }

        // Apply pre-selection, if any
        if (mPreSelectionMode != PreSelectNone) {
            mView->ignoreCurrentChanges(false);

            bool bSelectionDone = false;

            switch (mPreSelectionMode) {
            case PreSelectLastSelected:
                // fall down
                break;
            case PreSelectFirstUnreadCentered:
                bSelectionDone = mView->selectFirstMessageItem(MessageTypeUnreadOnly, true); // center
                break;
            case PreSelectOldestCentered:
                mView->setCurrentMessageItem(mOldestItem, true /* center */);
                bSelectionDone = true;
                break;
            case PreSelectNewestCentered:
                mView->setCurrentMessageItem(mNewestItem, true /* center */);
                bSelectionDone = true;
                break;
            case PreSelectNone:
                // deal with selection below
                break;
            default:
                qCWarning(MESSAGELIST_LOG) << "ERROR: Unrecognized pre-selection mode " << static_cast<int>(mPreSelectionMode);
                break;
            }

            if ((!bSelectionDone) && (mPreSelectionMode != PreSelectNone)) {
                // fallback to last selected, if possible
                if (mLastSelectedMessageInFolder) { // we found it in the loading process: select and jump out
                    mView->setCurrentMessageItem(mLastSelectedMessageInFolder);
                    bSelectionDone = true;
                }
            }

            if (bSelectionDone) {
                mLastSelectedMessageInFolder = nullptr;
                mPreSelectionMode = PreSelectNone;
                return; // already taken care of current / selection
            }
        }
        // deal with current/selection out of the switch

        break;
    default:
        // This is *really* a BUG
        qCWarning(MESSAGELIST_LOG) << "ERROR: returned an invalid result";
        Q_ASSERT(false);
        break;
    }

    // Everything else here deals with the selection

    // If UI is disconnected then we don't have anything else to do here
    if (!mModelForItemFunctions) {
        mView->ignoreCurrentChanges(false);
        return;
    }

    // Restore current/selection and/or scrollbar position

    if (mCurrentItemToRestoreAfterViewItemJobStep) {
        bool stillIgnoringCurrentChanges = true;

        // If the assert below fails then the previously current item got detached
        // and didn't get reattached in the step: this should never happen.
        Q_ASSERT(mCurrentItemToRestoreAfterViewItemJobStep->isViewable());

        // Check if the current item changed
        QModelIndex currentIndexAfterStep = mView->currentIndex();
        Item *currentAfterStep = currentIndexAfterStep.isValid() ? static_cast<Item *>(currentIndexAfterStep.internalPointer()) : nullptr;

        if (mCurrentItemToRestoreAfterViewItemJobStep != currentAfterStep) {
            // QTreeView lost the current item...
            if (mCurrentItemToRestoreAfterViewItemJobStep != currentItemBeforeStep) {
                // Some view job code expects us to actually *change* the current item.
                // This is done by the cleanup step which removes items and tries
                // to set as current the item *after* the removed one, if possible.
                // We need the view to handle the change though.
                stillIgnoringCurrentChanges = false;
                mView->ignoreCurrentChanges(false);
            } else {
                // we just have to restore the old current item. The code
                // outside shouldn't have noticed that we lost it (e.g. the message viewer
                // still should have the old message opened). So we don't need to
                // actually notify the view of the restored setting.
            }
            // Restore it
            qCDebug(MESSAGELIST_LOG) << "Gonna restore current here" << mCurrentItemToRestoreAfterViewItemJobStep->subject();
            mView->setCurrentIndex(q->index(mCurrentItemToRestoreAfterViewItemJobStep, 0));
        } else {
            // The item we're expected to set as current is already current
            if (mCurrentItemToRestoreAfterViewItemJobStep != currentItemBeforeStep) {
                // But we have changed it in the job step.
                // This means that: we have deleted the current item and chosen a
                // new candidate as current but Qt also has chosen it as candidate
                // and already made it current. The problem is that (as of Qt 4.4)
                // it probably didn't select it.
                if (!mView->selectionModel()->hasSelection()) {
                    stillIgnoringCurrentChanges = false;
                    mView->ignoreCurrentChanges(false);

                    qCDebug(MESSAGELIST_LOG) << "Gonna restore selection here" << mCurrentItemToRestoreAfterViewItemJobStep->subject();

                    QItemSelection selection;
                    selection.append(QItemSelectionRange(q->index(mCurrentItemToRestoreAfterViewItemJobStep, 0)));
                    mView->selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
                }
            }
        }

        // FIXME: If it was selected before the change, then re-select it (it may happen that it's not)
        if (lockViewDirection == 0) {
            // we prefer to keep the currently selected item steady in the view
            QRect rectAfterViewItemJobStep = mView->visualRect(q->index(mCurrentItemToRestoreAfterViewItemJobStep, 0));
            if (rectBeforeViewItemJobStep.y() != rectAfterViewItemJobStep.y()) {
                // QTreeView lost its position...
                mView->verticalScrollBar()->setValue(mView->verticalScrollBar()->value() + rectAfterViewItemJobStep.y() - rectBeforeViewItemJobStep.y());
            }
        }

        // and kill the insulation, if not yet done
        if (stillIgnoringCurrentChanges) {
            mView->ignoreCurrentChanges(false);
        }

        return;
    }

    // Either there was no current item before, or it was lost in a cleanup step and another candidate for
    // current item couldn't be found (possibly empty view)
    mView->ignoreCurrentChanges(false);

    if (currentItemBeforeStep) {
        // lost in a cleanup..
        // tell the view that we have a new current, this time with no insulation
        mView->slotSelectionChanged(QItemSelection(), QItemSelection());
    }
}

void ModelPrivate::slotStorageModelRowsInserted(const QModelIndex &parent, int from, int to)
{
    if (parent.isValid()) {
        return; // ugh... should never happen
    }

    Q_ASSERT(from <= to);

    int count = (to - from) + 1;

    mInvariantRowMapper->modelRowsInserted(from, count);

    // look if no current job is in the middle

    int jobCount = mViewItemJobs.count();

    for (int idx = 0; idx < jobCount; idx++) {
        ViewItemJob *job = mViewItemJobs.at(idx);

        if (job->currentPass() != ViewItemJob::Pass1Fill) {
            // The job is a cleanup or in a later pass: the storage has been already accessed
            // and the messages created... no need to care anymore: the invariant row mapper will do the job.
            continue;
        }

        if (job->currentIndex() > job->endIndex()) {
            // The job finished the Pass1Fill but still waits for the pass indicator to be
            // changed. This is unlikely but still may happen if the job has been interrupted
            // and then a call to slotStorageModelRowsRemoved() caused it to be forcibly completed.
            continue;
        }

        //
        // The following cases are possible:
        //
        //               from  to
        //                 |    |                              -> shift up job
        //               from             to
        //                 |              |                    -> shift up job
        //               from                            to
        //                 |                             |     -> shift up job
        //                           from   to
        //                             |     |                 -> split job
        //                           from                to
        //                             |                 |     -> split job
        //                                     from      to
        //                                       |       |     -> job unaffected
        //
        //
        // FOLDER
        // |-------------------------|---------|--------------|
        // 0                   currentIndex endIndex         count
        //                           +-- job --+
        //

        if (from > job->endIndex()) {
            // The change is completely above the job, the job is not affected
            continue;
        }

        if (from > job->currentIndex()) { // and from <= job->endIndex()
            // The change starts in the middle of the job in a way that it must be split in two.
            // The first part is unaffected by the shift and ranges from job->currentIndex() to from - 1.
            // The second part ranges from "from" to job->endIndex() that are now shifted up by count steps.

            // First add a new job for the second part.
            auto newJob = new ViewItemJob(from + count, job->endIndex() + count, job->chunkTimeout(), job->idleInterval(), job->messageCheckCount());

            Q_ASSERT(newJob->currentIndex() <= newJob->endIndex());

            idx++; // we can skip this job in the loop, it's already ok
            jobCount++; // and our range increases by one.
            mViewItemJobs.insert(idx, newJob);

            // Then limit the original job to the first part
            job->setEndIndex(from - 1);

            Q_ASSERT(job->currentIndex() <= job->endIndex());

            continue;
        }

        // The change starts below (or exactly on the beginning of) the job.
        // The job must be shifted up.
        job->setCurrentIndex(job->currentIndex() + count);
        job->setEndIndex(job->endIndex() + count);

        Q_ASSERT(job->currentIndex() <= job->endIndex());
    }

    bool newJobNeeded = true;

    // Try to attach to an existing fill job, if any.
    // To enforce consistency we can attach only if the Fill job
    // is the last one in the list (might be eventually *also* the first,
    // and even being already processed but we must make sure that there
    // aren't jobs _after_ it).
    if (jobCount > 0) {
        ViewItemJob *job = mViewItemJobs.at(jobCount - 1);
        if (job->currentPass() == ViewItemJob::Pass1Fill) {
            if (
                // The job ends just before the added rows
                (from == (job->endIndex() + 1)) && // The job didn't reach the end of Pass1Fill yet
                (job->currentIndex() <= job->endIndex())) {
                // We can still attach this :)
                job->setEndIndex(to);
                Q_ASSERT(job->currentIndex() <= job->endIndex());
                newJobNeeded = false;
            }
        }
    }

    if (newJobNeeded) {
        // FIXME: Should take timing options from aggregation here ?
        auto job = new ViewItemJob(from, to, 100, 50, 10);
        mViewItemJobs.append(job);
    }

    if (!mFillStepTimer.isActive()) {
        mFillStepTimer.start(mViewItemJobStepIdleInterval);
    }
}

void ModelPrivate::slotStorageModelRowsRemoved(const QModelIndex &parent, int from, int to)
{
    // This is called when the underlying StorageModel emits the rowsRemoved signal.

    if (parent.isValid()) {
        return; // ugh... should never happen
    }

    // look if no current job is in the middle

    Q_ASSERT(from <= to);

    const int count = (to - from) + 1;

    int jobCount = mViewItemJobs.count();

    if (mRootItem && from == 0 && count == mRootItem->childItemCount() && jobCount == 0) {
        clear();
        return;
    }

    for (int idx = 0; idx < jobCount; idx++) {
        ViewItemJob *job = mViewItemJobs.at(idx);

        if (job->currentPass() != ViewItemJob::Pass1Fill) {
            // The job is a cleanup or in a later pass: the storage has been already accessed
            // and the messages created... no need to care: we will invalidate the messages in a while.
            continue;
        }

        if (job->currentIndex() > job->endIndex()) {
            // The job finished the Pass1Fill but still waits for the pass indicator to be
            // changed. This is unlikely but still may happen if the job has been interrupted
            // and then a call to slotStorageModelRowsRemoved() caused it to be forcibly completed.
            continue;
        }

        //
        // The following cases are possible:
        //
        //               from  to
        //                 |    |                              -> shift down job
        //               from             to
        //                 |              |                    -> shift down and crop job
        //               from                            to
        //                 |                             |     -> kill job
        //                           from   to
        //                             |     |                 -> split job, crop and shift
        //                           from                to
        //                             |                 |     -> crop job
        //                                     from      to
        //                                       |       |     -> job unaffected
        //
        //
        // FOLDER
        // |-------------------------|---------|--------------|
        // 0                   currentIndex endIndex         count
        //                           +-- job --+
        //

        if (from > job->endIndex()) {
            // The change is completely above the job, the job is not affected
            continue;
        }

        if (from > job->currentIndex()) { // and from <= job->endIndex()
            // The change starts in the middle of the job and ends in the middle or after the job.
            // The first part is unaffected by the shift and ranges from job->currentIndex() to from - 1
            // We use the existing job for this.
            job->setEndIndex(from - 1); // stop before the first removed row

            Q_ASSERT(job->currentIndex() <= job->endIndex());

            if (to < job->endIndex()) {
                // The change ends inside the job and a part of it can be completed.

                // We create a new job for the shifted remaining part. It would actually
                // range from to + 1 up to job->endIndex(), but we need to shift it down by count.
                // since count = ( to - from ) + 1 so from = to + 1 - count

                auto newJob = new ViewItemJob(from, job->endIndex() - count, job->chunkTimeout(), job->idleInterval(), job->messageCheckCount());

                Q_ASSERT(newJob->currentIndex() < newJob->endIndex());

                idx++; // we can skip this job in the loop, it's already ok
                jobCount++; // and our range increases by one.
                mViewItemJobs.insert(idx, newJob);
            } // else the change includes completely the end of the job and no other part of it can be completed.

            continue;
        }

        // The change starts below (or exactly on the beginning of) the job. ( from <= job->currentIndex() )
        if (to >= job->endIndex()) {
            // The change completely covers the job: kill it

            // We don't delete the job since we want the other passes to be completed
            // This is because the Pass1Fill may have already filled mUnassignedMessageListForPass2
            // and may have set mOldestItem and mNewestItem. We *COULD* clear the unassigned
            // message list with clearUnassignedMessageLists() but mOldestItem and mNewestItem
            // could be still dangling pointers. So we just move the current index of the job
            // after the end (so storage model scan terminates) and let it complete spontaneously.
            job->setCurrentIndex(job->endIndex() + 1);

            continue;
        }

        if (to >= job->currentIndex()) {
            // The change partially covers the job. Only a part of it can be completed
            // and it must be shifted down. It would actually
            // range from to + 1 up to job->endIndex(), but we need to shift it down by count.
            // since count = ( to - from ) + 1 so from = to + 1 - count
            job->setCurrentIndex(from);
            job->setEndIndex(job->endIndex() - count);

            Q_ASSERT(job->currentIndex() <= job->endIndex());

            continue;
        }

        // The change is completely below the job: it must be shifted down.
        job->setCurrentIndex(job->currentIndex() - count);
        job->setEndIndex(job->endIndex() - count);
    }

    // This will invalidate the ModelInvariantIndex-es that have been removed and return
    // them all in a nice list that we can feed to a view removal job.
    auto invalidatedIndexes = mInvariantRowMapper->modelRowsRemoved(from, count);

    if (invalidatedIndexes) {
        // Try to attach to an existing cleanup job, if any.
        // To enforce consistency we can attach only if the Cleanup job
        // is the last one in the list (might be eventually *also* the first,
        // and even being already processed but we must make sure that there
        // aren't jobs _after_ it).
        if (jobCount > 0) {
            ViewItemJob *job = mViewItemJobs.at(jobCount - 1);
            if (job->currentPass() == ViewItemJob::Pass1Cleanup) {
                if ((job->currentIndex() <= job->endIndex()) && job->invariantIndexList()) {
                    // qCDebug(MESSAGELIST_LOG) << "Appending " << invalidatedIndexes->count() << " invalidated indexes to existing cleanup job";
                    // We can still attach this :)
                    *(job->invariantIndexList()) += *invalidatedIndexes;
                    job->setEndIndex(job->endIndex() + invalidatedIndexes->count());
                    delete invalidatedIndexes;
                    invalidatedIndexes = nullptr;
                }
            }
        }

        if (invalidatedIndexes) {
            // Didn't append to any existing cleanup job.. create a new one

            // qCDebug(MESSAGELIST_LOG) << "Creating new cleanup job for " << invalidatedIndexes->count() << " invalidated indexes";
            // FIXME: Should take timing options from aggregation here ?
            auto job = new ViewItemJob(ViewItemJob::Pass1Cleanup, invalidatedIndexes, 100, 50, 10);
            mViewItemJobs.append(job);
        }

        if (!mFillStepTimer.isActive()) {
            mFillStepTimer.start(mViewItemJobStepIdleInterval);
        }
    }
}

void ModelPrivate::slotStorageModelLayoutChanged()
{
    qCDebug(MESSAGELIST_LOG) << "Storage model layout changed";
    // need to reset everything...
    q->setStorageModel(mStorageModel);
    qCDebug(MESSAGELIST_LOG) << "Storage model layout changed done";
}

void ModelPrivate::slotStorageModelDataChanged(const QModelIndex &fromIndex, const QModelIndex &toIndex)
{
    Q_ASSERT(mStorageModel); // must exist (and be the sender of the signal connected to this slot)

    int from = fromIndex.row();
    int to = toIndex.row();

    Q_ASSERT(from <= to);

    int count = (to - from) + 1;

    int jobCount = mViewItemJobs.count();

    // This will find out the ModelInvariantIndex-es that need an update and will return
    // them all in a nice list that we can feed to a view removal job.
    auto indexesThatNeedUpdate = mInvariantRowMapper->modelIndexRowRangeToModelInvariantIndexList(from, count);

    if (indexesThatNeedUpdate) {
        // Try to attach to an existing update job, if any.
        // To enforce consistency we can attach only if the Update job
        // is the last one in the list (might be eventually *also* the first,
        // and even being already processed but we must make sure that there
        // aren't jobs _after_ it).
        if (jobCount > 0) {
            ViewItemJob *job = mViewItemJobs.at(jobCount - 1);
            if (job->currentPass() == ViewItemJob::Pass1Update) {
                if ((job->currentIndex() <= job->endIndex()) && job->invariantIndexList()) {
                    // We can still attach this :)
                    *(job->invariantIndexList()) += *indexesThatNeedUpdate;
                    job->setEndIndex(job->endIndex() + indexesThatNeedUpdate->count());
                    delete indexesThatNeedUpdate;
                    indexesThatNeedUpdate = nullptr;
                }
            }
        }

        if (indexesThatNeedUpdate) {
            // Didn't append to any existing update job.. create a new one
            // FIXME: Should take timing options from aggregation here ?
            auto job = new ViewItemJob(ViewItemJob::Pass1Update, indexesThatNeedUpdate, 100, 50, 10);
            mViewItemJobs.append(job);
        }

        if (!mFillStepTimer.isActive()) {
            mFillStepTimer.start(mViewItemJobStepIdleInterval);
        }
    }
}

void ModelPrivate::slotStorageModelHeaderDataChanged(Qt::Orientation, int, int)
{
    if (mStorageModelContainsOutboundMessages != mStorageModel->containsOutboundMessages()) {
        mStorageModelContainsOutboundMessages = mStorageModel->containsOutboundMessages();
        Q_EMIT q->headerDataChanged(Qt::Horizontal, 0, q->columnCount());
    }
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    Q_ASSERT(d->mModelForItemFunctions); // UI must be connected if a valid index was queried

    Item *it = static_cast<Item *>(index.internalPointer());

    Q_ASSERT(it);

    if (it->type() == Item::GroupHeader) {
        return Qt::ItemIsEnabled;
    }

    Q_ASSERT(it->type() == Item::Message);

    if (!static_cast<MessageItem *>(it)->isValid()) {
        return Qt::NoItemFlags; // not enabled, not selectable
    }

    if (static_cast<MessageItem *>(it)->aboutToBeRemoved()) {
        return Qt::NoItemFlags; // not enabled, not selectable
    }

    if (static_cast<MessageItem *>(it)->status().isDeleted()) {
        return Qt::NoItemFlags; // not enabled, not selectable
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QMimeData *MessageList::Core::Model::mimeData(const QModelIndexList &indexes) const
{
    QList<MessageItem *> msgs;
    for (const QModelIndex &idx : indexes) {
        if (idx.isValid()) {
            Item *item = static_cast<Item *>(idx.internalPointer());
            if (item->type() == MessageList::Core::Item::Message) {
                msgs << static_cast<MessageItem *>(idx.internalPointer());
            }
        }
    }
    return storageModel()->mimeData(msgs);
}

Item *Model::rootItem() const
{
    return d->mRootItem;
}

bool Model::isLoading() const
{
    return d->mLoading;
}

MessageItem *Model::messageItemByStorageRow(int row) const
{
    if (!d->mStorageModel) {
        return nullptr;
    }
    auto idx = d->mInvariantRowMapper->modelIndexRowToModelInvariantIndex(row);
    if (!idx) {
        return nullptr;
    }

    return static_cast<MessageItem *>(idx);
}

MessageItemSetReference Model::createPersistentSet(const QList<MessageItem *> &items)
{
    if (!d->mPersistentSetManager) {
        d->mPersistentSetManager = new MessageItemSetManager();
    }

    MessageItemSetReference ref = d->mPersistentSetManager->createSet();
    for (const auto mi : items) {
        d->mPersistentSetManager->addMessageItem(ref, mi);
    }

    return ref;
}

QList<MessageItem *> Model::persistentSetCurrentMessageItemList(MessageItemSetReference ref)
{
    if (d->mPersistentSetManager) {
        return d->mPersistentSetManager->messageItems(ref);
    }
    return {};
}

void Model::deletePersistentSet(MessageItemSetReference ref)
{
    if (!d->mPersistentSetManager) {
        return;
    }

    d->mPersistentSetManager->removeSet(ref);

    if (d->mPersistentSetManager->setCount() < 1) {
        delete d->mPersistentSetManager;
        d->mPersistentSetManager = nullptr;
    }
}

#include "moc_model.cpp"
