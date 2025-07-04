/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "model.h"
#include "threadingcache.h"
#include <QTimer>
#include <config-messagelist.h>

class QElapsedTimer;
namespace MessageList
{
namespace Core
{
class ViewItemJob;
class ModelInvariantRowMapper;
class MessageItemSetManager;
class ModelPrivate
{
public:
    explicit ModelPrivate(Model *owner)
        : q(owner)
    {
    }

    void fillView();

    /**
     * This is called by MessageList::Manager once in a while.
     * It is a good place to check if the date has changed and
     * trigger a view reload.
     */
    void checkIfDateChanged();

    void viewItemJobStep();

    /**
     * Attempt to find the threading parent for the specified message item.
     * Sets the message threading status to the appropriate value.
     *
     * This function performs In-Reply-To and References threading.
     */
    MessageItem *findMessageParent(MessageItem *mi);
    /**
     * Attempt to find the threading parent for the specified message item.
     * Sets the message threading status to the appropriate value.
     *
     * This function performs Subject based threading.
     */
    MessageItem *guessMessageParent(MessageItem *mi);

    enum AttachOptions {
        SkipCacheUpdate = 0,
        StoreInCache = 1
    };
    void attachMessageToParent(Item *pParent, MessageItem *mi, AttachOptions attachOptions = StoreInCache);
    void messageDetachedUpdateParentProperties(Item *oldParent, MessageItem *mi);
    void attachMessageToGroupHeader(MessageItem *mi);
    void attachGroup(GroupHeaderItem *ghi);

    enum ViewItemJobResult {
        ViewItemJobCompleted,
        ViewItemJobInterrupted,
    };
    ViewItemJobResult viewItemJobStepInternal();
    ViewItemJobResult viewItemJobStepInternalForJob(ViewItemJob *job, QElapsedTimer elapsedTimer);

    // FIXME: Those look like they should be made virtual in some job class! -> Refactor
    ViewItemJobResult viewItemJobStepInternalForJobPass1Fill(ViewItemJob *job, QElapsedTimer elapsedTimer);
    ViewItemJobResult viewItemJobStepInternalForJobPass1Cleanup(ViewItemJob *job, QElapsedTimer elapsedTimer);
    ViewItemJobResult viewItemJobStepInternalForJobPass1Update(ViewItemJob *job, QElapsedTimer elapsedTimer);
    ViewItemJobResult viewItemJobStepInternalForJobPass2(ViewItemJob *job, QElapsedTimer elapsedTimer);
    ViewItemJobResult viewItemJobStepInternalForJobPass3(ViewItemJob *job, QElapsedTimer elapsedTimer);
    ViewItemJobResult viewItemJobStepInternalForJobPass4(ViewItemJob *job, QElapsedTimer elapsedTimer);
    ViewItemJobResult viewItemJobStepInternalForJobPass5(ViewItemJob *job, QElapsedTimer elapsedTimer);
    void clearJobList();
    void clearUnassignedMessageLists();
    void clearOrphanChildrenHash();
    void clearThreadingCacheReferencesIdMD5ToMessageItem();
    void clearThreadingCacheMessageSubjectMD5ToMessageItem();
    void addMessageToReferencesBasedThreadingCache(MessageItem *mi);
    void removeMessageFromReferencesBasedThreadingCache(MessageItem *mi);
    void addMessageToSubjectBasedThreadingCache(MessageItem *mi);
    void removeMessageFromSubjectBasedThreadingCache(MessageItem *mi);
    void clear();
    /**
     * Expand the view while temporarily disabling animations - for expansion *not* triggered by the user
     * clicking on expand, which is often a mass operation, and animations are expensive.
     */
    void expandViewNoninteractive(const QModelIndex &index);
    /**
     * Sync the expanded state of the subtree with the specified root.
     * This will cause the items that are marked with Item::ExpandNeeded to be
     * expanded also in the view. For optimization purposes the specified root
     * is assumed to be marked as Item::ExpandNeeded so be sure to check it
     * before calling this function.
     */
    void syncExpandedStateOfSubtree(Item *root);
    /**
     * Save the expanded state of the subtree with the specified root.
     * The state will be saved in the initialExpandStatus() variable.
     * For optimization purposes the specified root is assumed to be expanded
     * and viewable.
     */
    void saveExpandedStateOfSubtree(Item *root);

#ifdef KDEPIM_FOLDEROPEN_PROFILE
    // This prints out all the stats we collected
    void printStatistics();
#endif

    enum PropertyChanges {
        DateChanged = 1,
        MaxDateChanged = (1 << 1),
        ActionItemStatusChanged = (1 << 2),
        UnreadStatusChanged = (1 << 3),
        ImportantStatusChanged = (1 << 4),
        AttachmentStatusChanged = (1 << 5),
    };

    /**
     * Handle the specified property changes in item. Depending on the item
     * position inside the parent and the types of item and parent the item
     * might need re-grouping or re-sorting. This function takes care of that.
     * It is meant to be called from somewhere inside viewItemJobStepInternal()
     * as it postpones group updates to Pass5.
     *
     * parent and item must not be null. propertyChangeMask should not be zero.
     *
     * Return true if parent might be affected by the item property changes
     * and false otherwise.
     */
    bool handleItemPropertyChanges(int propertyChangeMask, Item *parent, Item *item);

    /**
     * This one checks if the parent of item requires an update due to the
     * properties of item (that might have been changed or the item might
     * have been simply added to the parent). The properties
     * are propagated up to the root item. As optimization we ASSUME that
     * the item->parent() exists (is non 0) and is NOT the root item.
     * Be sure to check it before calling this function (it will assert in debug mode anyway).
     * ... ah... and don't be afraid: this is NOT (directly) recursive :)
     */
    void propagateItemPropertiesToParent(Item *item);

    /**
     * Recursively applies the current filter to the tree originating at the specified item.
     * The item is hidden if the filter doesn't match (the item or any children of it)
     * and this function returns false.
     * If the filter matches somewhere in the subtree then the item isn't hidden
     * and this function returns true.
     *
     * Assumes that the specified item is viewable.
     */
    bool applyFilterToSubtree(Item *item, const QModelIndex &parentIndex);

    // Slots connected to the underlying StorageModel.

    void slotStorageModelRowsInserted(const QModelIndex &parent, int from, int to);
    void slotStorageModelRowsRemoved(const QModelIndex &parent, int from, int to);
    void slotStorageModelDataChanged(const QModelIndex &fromIndex, const QModelIndex &toIndex);
    void slotStorageModelHeaderDataChanged(Qt::Orientation orientation, int first, int last);
    void slotStorageModelLayoutChanged();
    void slotApplyFilter();

    Model *const q;

    /** counter to avoid infinite recursions in the setStorageModel() function */
    int mRecursionCounterForReset;

    /**
     * The currently set storage model: shallow pointer.
     */
    StorageModel *mStorageModel = nullptr;

    /**
     * The currently set aggregation mode: shallow pointer set by Widget
     */
    const Aggregation *mAggregation = nullptr;

    /**
     * The currently used theme: shallow pointer
     */
    const Theme *mTheme = nullptr;

    /**
     * The currently used sort order. Pointer not owned by us, but by the Widget.
     */
    const SortOrder *mSortOrder = nullptr;

    /**
     * The filter to apply on messages. Shallow. Never 0.
     */
    const Filter *mFilter = nullptr;

    /**
     * The timer involved in breaking the "fill" operation in steps
     */
    QTimer mFillStepTimer;

    /**
     * Group Key (usually the label) -> GroupHeaderItem, used to quickly find groups, pointers are shallow copies
     */
    QHash<QString, GroupHeaderItem *> mGroupHeaderItemHash;

    /**
     * Threading cache.
     * MessageIdMD5 -> MessageItem, pointers are shallow copies
     */
    QHash<MD5Hash, MessageItem *> mThreadingCacheMessageIdMD5ToMessageItem;

    /**
     * Threading cache.
     * MessageInReplyToIdMD5 -> MessageItem, pointers are shallow copies
     */
    QMultiHash<MD5Hash, MessageItem *> mThreadingCacheMessageInReplyToIdMD5ToMessageItem;

    /**
     * Threading cache.
     * ReferencesIdMD5 -> MessageItem, pointers are shallow copies
     */
    QHash<MD5Hash, QList<MessageItem *> *> mThreadingCacheMessageReferencesIdMD5ToMessageItem;

    /**
     * Threading cache.
     * SubjectMD5 -> MessageItem, pointers are shallow copies
     */
    QHash<MD5Hash, QList<MessageItem *> *> mThreadingCacheMessageSubjectMD5ToMessageItem;

    /**
     * List of group headers that either need to be re-sorted or must be removed because empty
     */
    QHash<GroupHeaderItem *, GroupHeaderItem *> mGroupHeadersThatNeedUpdate;

    /**
     * List of unassigned messages, used to handle threading in two passes, pointers are owned!
     */
    QList<MessageItem *> mUnassignedMessageListForPass2;

    /**
     * List of unassigned messages, used to handle threading in two passes, pointers are owned!
     */
    QList<MessageItem *> mUnassignedMessageListForPass3;

    /**
     * List of unassigned messages, used to handle threading in two passes, pointers are owned!
     */
    QList<MessageItem *> mUnassignedMessageListForPass4;

    /**
     * Hash of orphan children used in Pass1Cleanup.
     */
    QHash<MessageItem *, MessageItem *> mOrphanChildrenHash;

    /**
     * Pending fill view jobs, pointers are owned
     */
    QList<ViewItemJob *> mViewItemJobs;

    /**
     * The today's date. Set when the StorageModel is set and thus grouping is performed.
     * This is used to put the today's messages in the "Today" group, for instance.
     */
    QDate mTodayDate;

    /**
     * Owned invisible root item, useful to implement algorithms that not need
     * to handle the special case of parentless items. This is never 0.
     */
    Item *mRootItem = nullptr;

    /**
     * The view we're attached to. Shallow pointer (the View owns us).
     */
    View *mView = nullptr;

    /**
     * The time at the current ViewItemJob step started. Used to compute the time we
     * spent inside this step and eventually jump out on timeout.
     */
    time_t mViewItemJobStepStartTime;

    /**
     * The timeout for a single ViewItemJob step
     */
    int mViewItemJobStepChunkTimeout;

    /**
     * The idle time between two ViewItemJob steps
     */
    int mViewItemJobStepIdleInterval;

    /**
     * The number of messages we process at once in a ViewItemJob step without
     * checking the timeouts above.
     */
    int mViewItemJobStepMessageCheckCount;

    /**
     * Our mighty ModelInvariantRowMapper: used to workaround an
     * issue related to the Model/View architecture.
     *
     * \sa ModelInvariantRowMapper
     */
    ModelInvariantRowMapper *mInvariantRowMapper = nullptr;

    /**
     * The label for the "Today" group item, cached, so we don't translate it multiple times.
     */
    QString mCachedTodayLabel;

    /**
     * The label for the "Yesterday" group item, cached, so we don't translate it multiple times.
     */
    QString mCachedYesterdayLabel;

    /**
     * The label for the "Unknown" group item, cached, so we don't translate it multiple times.
     */
    QString mCachedUnknownLabel;

    /**
     * The label for the "Last Week" group item, cached, so we don't translate it multiple times.
     */
    QString mCachedLastWeekLabel;

    /**
     * The label for the "Two Weeks Ago" group item, cached, so we don't translate it multiple times.
     */
    QString mCachedTwoWeeksAgoLabel;

    /**
     * The label for the "Three Weeks Ago" group item, cached, so we don't translate it multiple times.
     */
    QString mCachedThreeWeeksAgoLabel;

    /**
     * The label for the "Four Weeks Ago" group item, cached, so we don't translate it multiple times.
     */
    QString mCachedFourWeeksAgoLabel;

    /**
     * The label for the "Five Weeks Ago" group item, cached, so we don't translate it multiple times.
     */
    QString mCachedFiveWeeksAgoLabel;

    /**
     * Cached bits that we use for fast status checks
     */
    qint32 mCachedWatchedOrIgnoredStatusBits;

    /**
     * The labels for week days names group items, cached, so we don't query QLocale multiple times.
     */
    QMap<int, QString> mCachedDayNameLabel;

    /*
     * The labels for month names group items, cached, so we don't query QLocale multiple times.
     */
    QMap<int, QString> mCachedMonthNameLabel;

    /**
     * Flag signaling a possibly long job batch. This is checked by other
     * classes and used to display some kind of "please wait" feedback to the user.
     */
    bool mInLengthyJobBatch;

    /**
     * We need to save the current item before each job step. This is because
     * our job may cause items to be reparented (thus removed and read with the current Qt API)
     * and QTreeView will lose the current setting. We also use this to force the current
     * to a specific item after a cleanup job.
     */
    Item *mCurrentItemToRestoreAfterViewItemJobStep = nullptr;

    /**
     * Set to true in the first large loading job.
     * Reset to false when the job finishes.
     *
     * Please note that this is NOT set for later jobs: only for the first (possibly huge) one.
     */
    bool mLoading;

    /**
     * Pre-selection is the action of automatically selecting a message just after the folder
     * has finished loading. We may want to select the message that was selected the last
     * time this folder has been open, or we may want to select the first unread message.
     * We also may want to do no pre-selection at all (for example, when the user
     * starts navigating the view before the pre-selection could actually be made
     * and pre-selecting would confuse him). This member holds the option.
     *
     * See also setStorageModel() and abortMessagePreSelection()
     */
    PreSelectionMode mPreSelectionMode;

    // Oldest and newest item while loading the model
    // Not valid afterwards anymore. Used for pre-selection of the newest/oldest message
    MessageItem *mOldestItem = nullptr;
    MessageItem *mNewestItem = nullptr;

    /**
     * The id of the preselected ;essage is "translated" to a message pointer when it's fetched
     * from the storage. This message is then selected when it becomes viewable
     * (so at the end of the job). 0 if we have no message to select.
     *
     * See also setStorageModel() and abortMessagePreSelection()
     */
    MessageItem *mLastSelectedMessageInFolder = nullptr;

    /**
     * The "persistent message item sets" are (guess what?) sets of messages
     * that can be referenced globally via a persistent id. The MessageItemSetManager
     * and this class keep the persistent sets coherent: messages that are deleted
     * are automatically removed from all the sets.
     *
     * Users of this class typically create persistent sets when they start
     * an asynchronous job and they query them back on the way or when the job is terminated.
     *
     * So mPersistentSetManager is in fact the manager for the outstanding "user" jobs.
     * 0 if no jobs are pending (so there are no persistent sets at the moment).
     */
    MessageItemSetManager *mPersistentSetManager = nullptr;

    /**
     * This pointer is passed to the Item functions that insert children.
     * When we work with disconnected UI this pointer becomes 0.
     */
    Model *mModelForItemFunctions = nullptr;

    /**
     * The cached result of StorageModel::containsOutboundMessages().
     * We access this property at each incoming message and StorageModel::containsOutboundMessages() is
     * virtual (so it's always an indirect function call). Caching makes sense.
     */
    bool mStorageModelContainsOutboundMessages;

    /**
     * Vector of signal-slot connections between StorageModel and us
     */
    QList<QMetaObject::Connection> mStorageModelConnections;

    /**
     * Caches child - parent relation based on Akonadi ID and persists the cache
     * in a file for each Collection. This allows for very fast reconstruction of
     * threading.
     */
    ThreadingCache mThreadingCache;
};
} // namespace Core
} // namespace MessageList
