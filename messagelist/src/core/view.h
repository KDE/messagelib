/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QList>
#include <QPoint>
#include <QTreeView>

#include "messagelist/enums.h"
#include "messagelist/quicksearchline.h"
#include "messagelist/searchlinecommand.h"

class QMenu;

namespace Akonadi
{
class MessageStatus;
}

namespace MessageList
{
namespace Core
{
using MessageItemSetReference = long;

class Aggregation;
class Delegate;
class Item;
class MessageItem;
class Model;
class Theme;
class SortOrder;
class StorageModel;
class Widget;

/**
 * The MessageList::View is the real display of the message list. It is
 * based on QTreeView, has a Model that manipulates the underlying message storage
 * and a Delegate that is responsible for painting the items.
 */
class View : public QTreeView
{
    friend class Model;
    friend class ModelPrivate;
    Q_OBJECT
public:
    explicit View(Widget *parent);
    ~View() override;

    /**
     * Returns the Model attached to this View. You probably never need to manipulate
     * it directly.
     */
    [[nodiscard]] Model *model() const;

    /**
     * Returns the Delegate attached to this View. You probably never need to manipulate
     * it directly. Model uses it to obtain size hints.
     */
    [[nodiscard]] Delegate *delegate() const;

    /**
     * Sets the StorageModel to be displayed in this view. The StorageModel may be nullptr
     * (so no content is displayed).
     * Setting the StorageModel will obviously trigger a view reload.
     * Be sure to set the Aggregation and the Theme BEFORE calling this function.
     *
     * Pre-selection is the action of automatically selecting a message just after the folder
     * has finished loading. See Model::setStorageModel() for more information.
     */
    void setStorageModel(StorageModel *storageModel, PreSelectionMode preSelectionMode = PreSelectLastSelected);

    /**
     * Returns the currently displayed StorageModel. May be nullptr.
     */
    [[nodiscard]] StorageModel *storageModel() const;

    /**
     * Sets the aggregation for this view.
     * Does not trigger a reload of the view: you *MUST* trigger it manually.
     */
    void setAggregation(const Aggregation *aggregation);

    /**
     * Sets the specified theme for this view.
     * Does not trigger a reload of the view: you *MUST* trigger it manually.
     */
    void setTheme(Theme *theme);

    /**
     * Sets the specified sort order.
     * Does not trigger a reload of the view: you *MUST* trigger it manually.
     */
    void setSortOrder(const SortOrder *sortOrder);

    /**
     * Triggers a reload of the view in order to re-display the current folder.
     * Call this function after changing the Aggregation or the Theme.
     */
    void reload();

    /**
     * Returns the current MessageItem (that is bound to current StorageModel).
     * May return 0 if there is no current message or no current StorageModel.
     * If the current message item isn't currently selected (so is only focused)
     * then it's selected when this function is called, unless selectIfNeeded is false.
     */
    MessageItem *currentMessageItem(bool selectIfNeeded = true) const;

    /**
     * Returns the current Item (that is bound to current StorageModel).
     * May return 0 if there is no current item or no current StorageModel.
     * If the current item isn't currently selected (so is only focused)
     * then it's selected when this function is called.
     */
    [[nodiscard]] Item *currentItem() const;

    /**
     * Sets the current message item.
     */
    void setCurrentMessageItem(MessageItem *it, bool center = false);

    /**
     * Returns true if the specified item is currently displayed in the tree
     * and has all the parents expanded. This means that the user can
     * see the message (by eventually scrolling the view).
     */
    bool isDisplayedWithParentsExpanded(Item *it) const;

    /**
     * Makes sure that the specified is currently viewable by the user.
     * This means that the user can see the message (by eventually scrolling the view).
     */
    void ensureDisplayedWithParentsExpanded(Item *it);

    /**
     * Returns the currently selected MessageItems (bound to current StorageModel).
     * The list may be empty if there are no selected messages or no StorageModel.
     *
     * If includeCollapsedChildren is true then the children of the selected but
     * collapsed items are also added to the list.
     *
     * The returned list is guaranteed to be valid only until you return control
     * to the main even loop. Don't store it for any longer. If you need to reference
     * this set of messages at a later stage then take a look at createPersistentSet().
     */
    QList<MessageItem *> selectionAsMessageItemList(bool includeCollapsedChildren = true) const;

    /**
     * Returns the MessageItems bound to the current StorageModel that
     * are part of the current thread. The current thread is the thread
     * that contains currentMessageItem().
     * The list may be empty if there is no currentMessageItem() or no StorageModel.
     *
     * The returned list is guaranteed to be valid only until you return control
     * to the main even loop. Don't store it for any longer. If you need to reference
     * this set of messages at a later stage then take a look at createPersistentSet().
     */
    [[nodiscard]] QList<MessageItem *> currentThreadAsMessageItemList() const;

    /**
     * Fast function that determines if the selection is empty
     */
    [[nodiscard]] bool selectionEmpty() const;

    /**
     * Selects the specified MessageItems. The current selection is NOT removed.
     * Use clearSelection() for that purpose.
     */
    void selectMessageItems(const QList<MessageItem *> &list);

    /**
     * Creates a persistent set for the specified MessageItems and
     * returns its reference. Later you can use this reference
     * to retrieve the list of MessageItems that are still valid.
     * See persistentSetCurrentMessageList() for that.
     *
     * Persistent sets consume resources (both memory and CPU time
     * while manipulating the view) so be sure to call deletePersistentSet()
     * when you no longer need it.
     */
    MessageItemSetReference createPersistentSet(const QList<MessageItem *> &items);

    /**
     * Returns the list of MessageItems that are still existing in the
     * set pointed by the specified reference. This list will contain
     * at most the messages that you have passed to createPersistentSet()
     * but may contain less (even 0) if these MessageItem object were removed
     * from the view for some reason.
     */
    [[nodiscard]] QList<MessageItem *> persistentSetCurrentMessageItemList(MessageItemSetReference ref);

    /**
     * Deletes the persistent set pointed by the specified reference.
     * If the set does not exist anymore, nothing happens.
     */
    void deletePersistentSet(MessageItemSetReference ref);

    /**
     * If bMark is true this function marks the messages as "about to be removed"
     * so they appear dimmer and aren't selectable in the view.
     * If bMark is false then this function clears the "about to be removed" state
     * for the specified MessageItems.
     */
    void markMessageItemsAsAboutToBeRemoved(const QList<MessageItem *> &items, bool bMark);

    /**
     * Returns true if the current Aggregation is threaded, false otherwise
     * (or if there is no current Aggregation).
     */
    [[nodiscard]] bool isThreaded() const;

    /**
     * If expand is true then it expands the current thread, otherwise
     * collapses it.
     */
    void setCurrentThreadExpanded(bool expand);

    /**
     * If expand is true then it expands all the threads, otherwise
     * collapses them.
     */
    void setAllThreadsExpanded(bool expand);

    /**
     * If expand is true then it expands all the groups (only the toplevel
     * group item: inner threads are NOT expanded). If expand is false
     * then it collapses all the groups. If no grouping is in effect
     * then this function does nothing.
     */
    void setAllGroupsExpanded(bool expand);

    /**
     * Selects the next message item in the view.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     *
     * existingSelectionBehaviour specifies how the existing selection
     * is manipulated. It may be cleared, expanded or grown/shrunk.
     *
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     * If loop is true then the "next" algorithm will restart from the beginning
     * of the list if the end is reached, otherwise it will just stop returning false.
     *
     * \sa MessageList::Core::MessageTypeFilter
     * \sa MessageList::Core::ExistingSelectionBehaviour
     */
    bool selectNextMessageItem(MessageTypeFilter messageTypeFilter, ExistingSelectionBehaviour existingSelectionBehaviour, bool centerItem, bool loop);

    /**
     * Selects the previous message item in the view.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     *
     * existingSelectionBehaviour specifies how the existing selection
     * is manipulated. It may be cleared, expanded or grown/shrunk.
     *
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     * If loop is true then the "previous" algorithm will restart from the end
     * of the list if the beginning is reached, otherwise it will just stop returning false.
     *
     * \sa MessageList::Core::MessageTypeFilter
     * \sa MessageList::Core::ExistingSelectionBehaviour
     */
    bool selectPreviousMessageItem(MessageTypeFilter messageTypeFilter, ExistingSelectionBehaviour existingSelectionBehaviour, bool centerItem, bool loop);

    /**
     * Focuses the next message item in the view without actually selecting it.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     *
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     * If loop is true then the "next" algorithm will restart from the beginning
     * of the list if the end is reached, otherwise it will just stop returning false.
     */
    bool focusNextMessageItem(MessageTypeFilter messageTypeFilter, bool centerItem, bool loop);

    /**
     * Focuses the previous message item in the view without actually selecting it.
     * If unread is true then focuses the previous unread message item.
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     * If loop is true then the "previous" algorithm will restart from the end
     * of the list if the beginning is reached, otherwise it will just stop returning false.
     */
    bool focusPreviousMessageItem(MessageTypeFilter messageTypeFilter, bool centerItem, bool loop);

    /**
     * Selects the currently focused message item. If the currently focused
     * message is already selected (which is very likely) nothing happens.
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     */
    void selectFocusedMessageItem(bool centerItem);

    /**
     * Selects the first message item in the view that matches messageTypeFilter.
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     */
    bool selectFirstMessageItem(MessageTypeFilter messageTypeFilter, bool centerItem);

    /**
     * Selects the last message item in the view that matches messageTypeFilter.
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     */
    bool selectLastMessageItem(MessageTypeFilter messageTypeFilter, bool centerItem);

    /**
     * Sets the focus on the quick search line of the currently active tab.
     */
    void focusQuickSearch(const QString &selectedText);

    /**
     * Returns the Akonadi::MessageStatus in the current quicksearch field.
     */
    [[nodiscard]] QList<Akonadi::MessageStatus> currentFilterStatus() const;

    /**
     * Returns the search term in the current quicksearch field.
     */
    [[nodiscard]] QString currentFilterSearchString() const;

    /**
     * Called to hide or show the specified row from the view.
     * @reimp
     */
    virtual void setRowHidden(int row, const QModelIndex &parent, bool hide);

    void sortOrderMenuAboutToShow(QMenu *menu);

    void aggregationMenuAboutToShow(QMenu *menu);

    void themeMenuAboutToShow(QMenu *menu);

    void setCollapseItem(const QModelIndex &index);
    void setExpandItem(const QModelIndex &index);

    void setQuickSearchClickMessage(const QString &msg);

    [[nodiscard]] MessageList::Core::SearchMessageByButtons::SearchOptions currentOptions() const;

    [[nodiscard]] QList<SearchLineCommand::SearchLineInfo> searchLineCommands() const;

protected:
    /**
     * Reimplemented in order to catch QHelpEvent
     */
    bool event(QEvent *e) override;

    /**
     * Reimplemented in order to catch palette, font and style changes
     */
    void changeEvent(QEvent *e) override;

    /**
     * Reimplemented in order to handle clicks with sub-item precision.
     */
    void mousePressEvent(QMouseEvent *e) override;

    /**
     * Reimplemented in order to handle double clicks with sub-item precision.
     */
    void mouseDoubleClickEvent(QMouseEvent *e) override;

    /**
     * Reimplemented in order to handle DnD
     */
    void mouseMoveEvent(QMouseEvent *e) override;

    /**
     * Reimplemented in order to handle message DnD
     */
    void dragEnterEvent(QDragEnterEvent *e) override;

    /**
     * Reimplemented in order to handle message DnD
     */
    void dragMoveEvent(QDragMoveEvent *e) override;

    /**
     * Reimplemented in order to handle message DnD
     */
    void dropEvent(QDropEvent *e) override;

    /**
     * Reimplemented in order to resize columns when header is not visible
     */
    void resizeEvent(QResizeEvent *e) override;

    void paintEvent(QPaintEvent *event) override;
    /**
     * Reimplemented in order to kill the QTreeView column auto-resizing
     */
    [[nodiscard]] int sizeHintForColumn(int logicalColumnIndex) const override;

    /**
     * Reimplemented in order to disable update of the geometries
     * while a job step is running (as it takes a very long time and it's called for every item insertion...)
     * TODO: not true anymore, it's called after a delay.
     */
    void updateGeometries() override;

    /**
     * Returns -1 if the view should stay scrolled to the top, 1 if it should stay scrolled to the bottom,
     * and 0 if scrolling should not stay locked at one end.
     */
    int scrollingLockDirection() const;

    /**
     *  Used to enable/disable the ignoring of updateGeometries() calls.
     */
    void ignoreUpdateGeometries(bool ignore);

    void modelAboutToEmitLayoutChanged();
    void modelEmittedLayoutChanged();

    /**
     * This is called by the model to insulate us from certain QTreeView signals
     * This is because they may be spurious (caused by Model item rearrangements).
     */
    void ignoreCurrentChanges(bool ignore);

    /**
     * Expands or collapses the children of the specified item, recursively.
     */
    void setChildrenExpanded(const Item *parent, bool expand);

    /**
     * Finds the next message item with respect to the current item.
     * If there is no current item then the search starts from the beginning.
     * Returns 0 if no next message could be found.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     * If loop is true then restarts from the beginning if end is
     * reached, otherwise it just returns 0 in this case.
     */
    Item *nextMessageItem(MessageTypeFilter messageTypeFilter, bool loop);

    /**
     * Finds message item that comes "after" the reference item.
     * If reference item is 0 then the search starts from the beginning.
     * Returns 0 if no next message could be found.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     * If loop is true then restarts from the beginning if end is
     * reached, otherwise it just returns 0 in this case.
     */
    Item *messageItemAfter(Item *referenceItem, MessageTypeFilter messageTypeFilter, bool loop);

    /**
     * Finds the first message item in the view.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     *
     * Returns 0 if the view is empty.
     */
    Item *firstMessageItem(MessageTypeFilter messageTypeFilter);

    /**
     * Finds the previous message item with respect to the current item.
     * If there is no current item then the search starts from the end.
     * Returns 0 if no previous message could be found.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     * If loop is true then restarts from the end if beginning is
     * reached, otherwise it just return 0 in this case.
     */
    Item *previousMessageItem(MessageTypeFilter messageTypeFilter, bool loop);

    /**
     * Returns the deepest child that is visible (i.e. not in a collapsed tree) of
     * the specified reference item.
     */
    Item *deepestExpandedChild(Item *referenceItem) const;

    /**
     * Finds message item that comes "before" the reference item.
     * If reference item is 0 then the search starts from the end.
     * Returns 0 if no next message could be found.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     * If loop is true then restarts from the beginning if end is
     * reached, otherwise it just returns 0 in this case.
     */
    Item *messageItemBefore(Item *referenceItem, MessageTypeFilter messageTypeFilter, bool loop);

    /**
     * Finds the last message item in the view.
     *
     * messageTypeFilter can be used to limit the selection to
     * a certain category of messages.
     *
     * Returns nullptr if the view is empty.
     */
    Item *lastMessageItem(MessageTypeFilter messageTypeFilter);

    /**
     * This is called by Model to signal that the initial loading stage of a newly
     * attached StorageModel is terminated.
     */
    void modelFinishedLoading();

    /**
     * Performs a change in the specified MessageItem status.
     * It first applies the change to the cached state in MessageItem and
     * then requests our parent widget to act on the storage.
     */
    void changeMessageStatus(MessageItem *it, Akonadi::MessageStatus set, Akonadi::MessageStatus unset);
    void changeMessageStatusRead(MessageItem *it, bool read);

    /**
     * Starts a short-delay timer connected to saveThemeColumnState().
     * Used to accumulate consecutive changes and break out of the call stack
     * up to the main event loop (since in the call stack the column state might be left undefined).
     */
    void triggerDelayedSaveThemeColumnState();

    /**
     * Starts a short-delay timer connected to applyThemeColumns().
     * Used to accumulate consecutive changes and break out of the call stack
     * up to the main event loop (since multiple resize events tend to be sent by Qt at startup).
     */
    void triggerDelayedApplyThemeColumns();

    /**
     * This is used by the selection functions to grow/shrink the existing selection
     * according to the newly selected item passed as parameter.
     * If movingUp is true then: if the newly selected item is above the current selection top
     * then the selection is expanded, otherwise it's shrunk. If movingUp is false then: if the
     * newly selected item is below the current selection bottom then the selection is expanded
     * otherwise it's shrunk.
     */
    void growOrShrinkExistingSelection(const QModelIndex &newSelectedIndex, bool movingUp);

public Q_SLOTS:
    /**
     * Collapses all the group headers (if present in the current Aggregation)
     */
    void slotCollapseAllGroups();

    /**
     * Expands all the group headers (if present in the current Aggregation)
     */
    void slotExpandAllGroups();

    /**
     * Expands the current item.
     * If it's a Message, it expands its thread, if its a group header it expands the group
     */
    void slotExpandCurrentItem();

    /**
     * Collapses the current item.
     * If it's a Message, it collapses its thread, if its a group header it collapses the group
     */
    void slotCollapseCurrentItem();

    void slotExpandAllThreads();

    void slotCollapseAllThreads();

protected Q_SLOTS:

    /**
     * Handles context menu requests for the header.
     */
    void slotHeaderContextMenuRequested(const QPoint &pnt);

    /**
     * Handles the actions of the header context menu for showing/hiding a column.
     */
    void slotShowHideColumn(int columnIndex);

    /**
     * Handles the Adjust Column Sizes action of the header context menu.
     */
    void slotAdjustColumnSizes();

    /**
     * Handles the Show Default Columns action of the header context menu.
     */
    void slotShowDefaultColumns();

    /**
     * Handles the Display Tooltips action of the header context menu.
     */
    void slotDisplayTooltips(bool showTooltips);

    /**
     * Handles section resizes in order to save the column widths
     */
    void slotHeaderSectionResized(int logicalIndex, int oldWidth, int newWidth);

    /**
     * Handles selection item management
     */
    void slotSelectionChanged(const QItemSelection &current, const QItemSelection &);

    /**
     * Saves the state of the columns (width and visibility) to the currently selected theme object.
     */
    void saveThemeColumnState();

    /**
     * Applies the theme columns to this view.
     * Columns visible by default are shown, the other are hidden.
     * Visible columns are assigned space inside the view by using the size hints and some heuristics.
     */
    void applyThemeColumns();

private:
    class ViewPrivate;
    std::unique_ptr<ViewPrivate> const d;
}; // class View
} // namespace Core
} // namespace MessageList
