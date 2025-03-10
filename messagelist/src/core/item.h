/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QList>
#include <QString>

#include <ctime> // for time_t

#include <KMime/Headers>

#include <Akonadi/MessageStatus>

#include "core/model.h"
#include "messagelist_export.h"

namespace MessageList
{
namespace Core
{
class ItemPrivate;

/**
 * A single item of the MessageList tree managed by MessageList::Model.
 *
 * This class stores basic information needed in all the subclasses which
 * at the moment of writing are MessageItem and GroupHeaderItem.
 */
class MESSAGELIST_EXPORT Item
{
    friend class Model;
    friend class ModelPrivate;

public:
    /**
     * The type of the Item.
     */
    enum Type : uint8_t {
        GroupHeader, ///< This item is a GroupHeaderItem
        Message, ///< This item is a MessageItem
        InvisibleRoot ///< This item is just Item and it's the only InvisibleRoot per Model.
    };

    /**
     * Specifies the initial expand status for the item that should be applied
     * when it's attached to the viewable tree. Needed as a workaround for
     * QTreeView limitations in handling item expansion.
     */
    enum InitialExpandStatus : uint8_t {
        ExpandNeeded, ///< Must expand when this item becomes viewable
        NoExpandNeeded, ///< No expand needed at all
        ExpandExecuted ///< Item already expanded
    };

protected:
    /**
     * Creates an Item. Only derived classes and MessageList::Model should access this.
     */
    Item(Type type);
    Item(Type type, ItemPrivate *dd);

public:
    /**
     * Destroys the Item. Should be protected just like the constructor but the QList<>
     * helpers need to access it, so it's public actually.
     */
    virtual ~Item();

    /**
     * Returns the type of this item. The Type can be set only in the constructor.
     */
    [[nodiscard]] Type type() const;

    /**
     * The initial expand status we have to honor when attaching to the viewable root.
     */
    [[nodiscard]] InitialExpandStatus initialExpandStatus() const;

    /**
     * Set the initial expand status we have to honor when attaching to the viewable root.
     */
    void setInitialExpandStatus(InitialExpandStatus initialExpandStatus);

    /**
     * Is this item attached to the viewable root ?
     */
    [[nodiscard]] bool isViewable() const;

    /**
     * Return true if Item pointed by it is an ancestor of this item (that is,
     * if it is its parent, parent of its parent, parent of its parent of its parent etc...
     */
    [[nodiscard]] bool hasAncestor(const Item *it) const;

    /**
     * Makes this item viewable, that is, notifies its existence to any listener
     * attached to the "rowsInserted()" signal, most notably QTreeView.
     *
     * This will also make all the children viewable.
     */
    void setViewable(Model *model, bool bViewable);

    /**
     * Return the list of child items. May be null.
     */
    [[nodiscard]] QList<Item *> *childItems() const;

    /**
     * Returns the child item at position idx or 0 if idx is out of the allowable range.
     */
    [[nodiscard]] Item *childItem(int idx) const;

    /**
     * Returns the first child item, if any.
     */
    [[nodiscard]] Item *firstChildItem() const;

    /**
     * Returns the item that is visually below the specified child if this item.
     * Note that the returned item may belong to a completely different subtree.
     */
    [[nodiscard]] Item *itemBelowChild(Item *child);

    /**
     * Returns the item that is visually above the specified child if this item.
     * Note that the returned item may belong to a completely different subtree.
     */
    [[nodiscard]] Item *itemAboveChild(Item *child);

    /**
     * Returns the deepest item in the subtree originating at this item.
     */
    [[nodiscard]] Item *deepestItem();

    /**
     * Returns the item that is visually below this item in the tree.
     * Note that the returned item may belong to a completely different subtree.
     */
    [[nodiscard]] Item *itemBelow();

    /**
     * Returns the item that is visually above this item in the tree.
     * Note that the returned item may belong to a completely different subtree.
     */
    [[nodiscard]] Item *itemAbove();

    /**
     * Debug helper. Dumps the structure of this subtree.
     */
    void dump(const QString &prefix);

    /**
     * Returns the number of children of this Item.
     */
    [[nodiscard]] int childItemCount() const;

    /**
     * Convenience function that returns true if this item has children.
     */
    [[nodiscard]] bool hasChildren() const;

    /**
     * A structure used with MessageList::Item::childItemStats().
     * Contains counts of total and unread messages in a subtree.
     */
    class ChildItemStats
    {
    public:
        unsigned int mTotalChildCount; // total
        unsigned int mUnreadChildCount; // unread only
    public:
        ChildItemStats()
            : mTotalChildCount(0)
            , mUnreadChildCount(0)
        {
        }
    };

    /**
     * Gathers statistics about child items.
     * For performance purposes assumes that this item has children.
     * You MUST check it before calling it.
     */
    void childItemStats(ChildItemStats &stats) const;

    /**
     * Returns the actual index of the child Item item or -1 if
     * item is not a child of this Item.
     */
    int indexOfChildItem(Item *item) const;

    /**
     * Sets the cached guess for the index of this item in the parent's child list.
     *
     * This is used to speed up the index lookup with the following algorithm:
     * Ask the parent if this item is at the position specified by index guess (this costs ~O(1)).
     * If the position matches we have finished, if it doesn't then perform
     * a linear search via indexOfChildItem() (which costs ~O(n)).
     */
    void setIndexGuess(int index);

    /**
     * Returns the parent Item in the tree, or 0 if this item isn't attached to the tree.
     * Please note that even if this item has a non-zero parent, it can be still non viewable.
     * That is: the topmost parent of this item may be not attached to the viewable root.
     */
    [[nodiscard]] Item *parent() const;

    /**
     * Sets the parent for this item. You should also take care of inserting
     * this item in the parent's child list.
     */
    void setParent(Item *pParent);

    /**
     * Returns the topmost parent item that is not a Root item (that is, is a Message or GroupHeader).
     */
    [[nodiscard]] Item *topmostNonRoot();

    /**
     * Returns the status associated to this Item.
     */
    const Akonadi::MessageStatus &status() const;

    /**
     * Sets the status associated to this Item.
     */
    void setStatus(Akonadi::MessageStatus status);

    /**
     * Returns a string describing the status e.g: "Read, Forwarded, Important"
     */
    [[nodiscard]] QString statusDescription() const;

    /**
     * Returns the size of this item (size of the Message, mainly)
     */
    size_t size() const;

    /**
     * Sets the size of this item (size of the Message, mainly)
     */
    void setSize(size_t size);

    /**
     * A string with a text rappresentation of size(). This is computed on-the-fly
     * and not cached.
     */
    [[nodiscard]] QString formattedSize() const;

    /**
     * Returns the date of this item
     */
    time_t date() const;

    /**
     * Sets the date of this item
     */
    void setDate(time_t date);

    /**
     * A string with a text rappresentation of date() obtained via Manager. This is computed on-the-fly
     * and not cached.
     */
    [[nodiscard]] QString formattedDate() const;

    /**
     * Returns the maximum date in the subtree originating from this item.
     * This is kept up-to-date by MessageList::Model.
     */
    time_t maxDate() const;

    /**
     * Sets the maximum date in the subtree originating from this item.
     */
    void setMaxDate(time_t date);

    /**
     * A string with a text rappresentation of maxDate() obtained via Manager. This is computed on-the-fly
     * and not cached.
     */
    [[nodiscard]] QString formattedMaxDate() const;

    /**
     * Recompute the maximum date from the current children list.
     * Return true if the current max date changed and false otherwise.
     */
    bool recomputeMaxDate();

    /**
     * Returns the sender associated to this item.
     */
    [[nodiscard]] const QString &sender() const;

    /**
     * Sets the sender associated to this item.
     */
    void setSender(const QString &sender);

    /**
     * Display sender.
     */
    [[nodiscard]] QString displaySender() const;

    /**
     * Returns the receiver associated to this item.
     */
    const QString &receiver() const;

    /**
     * Sets the sender associated to this item.
     */
    void setReceiver(const QString &receiver);

    /**
     * Display receiver.
     */
    [[nodiscard]] QString displayReceiver() const;

    /**
     * Returns the sender or the receiver, depending on the underlying StorageModel settings.
     */
    const QString &senderOrReceiver() const;

    /**
     * Display sender or receiver.
     */
    [[nodiscard]] QString displaySenderOrReceiver() const;

    /**
     * Returns whether sender or receiver is supposed to be displayed.
     */
    bool useReceiver() const;

    /**
     * Returns the subject associated to this Item.
     */
    const QString &subject() const;

    /**
     * Sets the subject associated to this Item.
     */
    void setSubject(const QString &subject);

    /**
     * Returns the folder associated to this Item.
     */
    const QString &folder() const;

    /**
     * Sets the folder associated to this Item.
     */
    void setFolder(const QString &folder);

    /**
     * This is meant to be called right after the constructor.
     * It sets up several items at once (so even if not inlined it's still a single call)
     * and it skips some calls that can be avoided at constructor time.
     */
    void initialSetup(time_t date, size_t size, const QString &sender, const QString &receiver, bool useReceiver);

    void setItemId(qint64 id);
    [[nodiscard]] qint64 itemId() const;

    void setParentCollectionId(qint64 id);
    [[nodiscard]] qint64 parentCollectionId() const;

    /**
     * This is meant to be called right after the constructor for MessageItem objects.
     * It sets up several items at once (so even if not inlined it's still a single call).
     */
    void setSubjectAndStatus(const QString &subject, Akonadi::MessageStatus status);

    /**
     * Appends an Item to this item's child list.
     * The Model is used for beginInsertRows()/endInsertRows() calls.
     */
    int appendChildItem(Model *model, Item *child);

    /**
     * Appends a child item without inserting it via the model.
     * This is useful in ThemeEditor which doesn't use a custom model for the items.
     * You shouldn't need to use this function...
     */
    void rawAppendChildItem(Item *child);

    /**
     * Removes a child from this item's child list without deleting it.
     * The Model is used for beginRemoveRows()/endRemoveRows() calls.
     */
    void takeChildItem(Model *model, Item *child);

    /**
     * Kills all the child items without emitting any signal, recursively.
     * It should be used only when MessageList::Model is reset() afterwards.
     */
    void killAllChildItems();

protected:
    ItemPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(Item)
};
} // namespace Core
} // namespace MessageList
