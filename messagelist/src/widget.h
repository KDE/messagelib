/*
    SPDX-FileCopyrightText: 2009 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <MessageList/QuickSearchLine>
#include <MessageList/View>
#include <MessageList/WidgetBase>

#include <Akonadi/Item>

#include <KMime/Message>

#include "messagelist_export.h"
#include <QList>
class KXMLGUIClient;
class QWidget;
class KJob;
namespace MessageList
{
/**
 * The Akonadi specific implementation of the Core::Widget.
 *
 * When a KXmlGuiWindow is passed to setXmlGuiClient, the XMLGUI
 * defined context menu @c akonadi_messagelist_contextmenu is
 * used if available.
 *
 */
class MESSAGELIST_EXPORT Widget : public MessageList::Core::Widget
{
    Q_OBJECT

public:
    /**
     * Create a new message list widget.
     */
    explicit Widget(QWidget *parent);
    ~Widget() override;

    /**
     * Sets the XML GUI client which the view is used in.
     *
     * This is needed if you want to use the built-in context menu.
     * Passing 0 is ok and will disable the builtin context menu.
     *
     * @param xmlGuiClient The KXMLGUIClient the view is used in.
     */
    void setXmlGuiClient(KXMLGUIClient *xmlGuiClient);

    /**
     * Returns the current message for the list as Akonadi::Item.
     * May return an invalid Item if there is no current message or no current folder.
     */
    [[nodiscard]] Akonadi::Item currentItem() const;

    /**
     * Returns the current message for the list as KMime::Message::Ptr.
     * May return 0 if there is no current message or no current folder.
     */
    [[nodiscard]] KMime::Message::Ptr currentMessage() const;

    /**
     * Returns true if this drag can be accepted by the underlying view
     */
    [[nodiscard]] bool canAcceptDrag(const QDropEvent *e);

    /**
     * Selects the next message item in the view.
     *
     * messageTypeFilter can be used to restrict the selection to only certain message types.
     *
     * existingSelectionBehaviour specifies how the existing selection
     * is manipulated. It may be cleared, expanded or grown/shrunk.
     *
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     * If loop is true then the "next" algorithm will restart from the beginning
     * of the list if the end is reached, otherwise it will just stop returning false.
     */
    [[nodiscard]] bool selectNextMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter,
                                             MessageList::Core::ExistingSelectionBehaviour existingSelectionBehaviour,
                                             bool centerItem,
                                             bool loop);

    /**
     * Selects the previous message item in the view.
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     *
     * messageTypeFilter can be used to restrict the selection to only certain message types.
     *
     * existingSelectionBehaviour specifies how the existing selection
     * is manipulated. It may be cleared, expanded or grown/shrunk.
     *
     * If loop is true then the "previous" algorithm will restart from the end
     * of the list if the beginning is reached, otherwise it will just stop returning false.
     */
    [[nodiscard]] bool selectPreviousMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter,
                                                 MessageList::Core::ExistingSelectionBehaviour existingSelectionBehaviour,
                                                 bool centerItem,
                                                 bool loop);

    /**
     * Focuses the next message item in the view without actually selecting it.
     *
     * messageTypeFilter can be used to restrict the selection to only certain message types.
     *
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     * If loop is true then the "next" algorithm will restart from the beginning
     * of the list if the end is reached, otherwise it will just stop returning false.
     */
    [[nodiscard]] bool focusNextMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem, bool loop);

    /**
     * Focuses the previous message item in the view without actually selecting it.
     *
     * messageTypeFilter can be used to restrict the selection to only certain message types.
     *
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     * If loop is true then the "previous" algorithm will restart from the end
     * of the list if the beginning is reached, otherwise it will just stop returning false.
     */
    [[nodiscard]] bool focusPreviousMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem, bool loop);

    /**
     * Selects the currently focused message item. May do nothing if the
     * focused message item is already selected (which is very likely).
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     */
    void selectFocusedMessageItem(bool centerItem);

    /**
     * Selects the first message item in the view that matches the specified Core::MessageTypeFilter.
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     *
     * If the current view is already loaded then the request will
     * be satisfied immediately (well... if an unread message exists at all).
     * If the current view is still loading then the selection of the first
     * message will be scheduled to be executed when loading terminates.
     *
     * So this function doesn't actually guarantee that an unread or new message
     * was selected when the call returns. Take care :)
     *
     * The function returns true if a message was selected and false otherwise.
     */
    [[nodiscard]] bool selectFirstMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem);

    /**
     * Selects the last message item in the view that matches the specified Core::MessageTypeFilter.
     * If centerItem is true then the specified item will be positioned
     * at the center of the view, if possible.
     *
     * The function returns true if a message was selected and false otherwise.
     */
    [[nodiscard]] bool selectLastMessageItem(MessageList::Core::MessageTypeFilter messageTypeFilter, bool centerItem);

    /**
     * Selects all the items in the current folder.
     */
    void selectAll();
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
     * Sets the focus on the quick search line of the currently active tab.
     */
    void focusQuickSearch(const QString &selectedText);

    /**
     * Returns the currently selected KMime::Message::Ptr (bound to current StorageModel).
     * The list may be empty if there are no selected messages or no StorageModel.
     *
     * If includeCollapsedChildren is true then the children of the selected but
     * collapsed items are also added to the list.
     *
     * The returned list is guaranteed to be valid only until you return control
     * to the main even loop. Don't store it for any longer. If you need to reference
     * this set of messages at a later stage then take a look at createPersistentSet().
     */
    [[nodiscard]] QList<KMime::Message::Ptr> selectionAsMessageList(bool includeCollapsedChildren = true) const;

    /**
     * Returns the currently selected Items (bound to current StorageModel).
     * The list may be empty if there are no selected messages or no StorageModel.
     *
     * If includeCollapsedChildren is true then the children of the selected but
     * collapsed items are also added to the list.
     *
     * The returned list is guaranteed to be valid only until you return control
     * to the main even loop. Don't store it for any longer. If you need to reference
     * this set of messages at a later stage then take a look at createPersistentSet().
     */
    [[nodiscard]] Akonadi::Item::List selectionAsMessageItemList(bool includeCollapsedChildren = true) const;

    /**
     * Returns the currently selected Items id (bound to current StorageModel).
     * The list may be empty if there are no selected messages or no StorageModel.
     *
     * If includeCollapsedChildren is true then the children of the selected but
     * collapsed items are also added to the list.
     *
     * The returned list is guaranteed to be valid only until you return control
     * to the main even loop. Don't store it for any longer. If you need to reference
     * this set of messages at a later stage then take a look at createPersistentSet().
     */

    [[nodiscard]] QList<qlonglong> selectionAsMessageItemListId(bool includeCollapsedChildren) const;
    [[nodiscard]] QList<Akonadi::Item::Id> selectionAsListMessageId(bool includeCollapsedChildren) const;

    /**
     * Returns the Akonadi::Item bound to the current StorageModel that
     * are part of the current thread. The current thread is the thread
     * that contains currentMessageItem().
     * The list may be empty if there is no currentMessageItem() or no StorageModel.
     *
     * The returned list is guaranteed to be valid only until you return control
     * to the main even loop. Don't store it for any longer. If you need to reference
     * this set of messages at a later stage then take a look at createPersistentSet().
     */
    [[nodiscard]] Akonadi::Item::List currentThreadAsMessageList() const;

    /**
     * Returns the Akonadi::MessageStatus in the current quicksearch field.
     */
    [[nodiscard]] QList<Akonadi::MessageStatus> currentFilterStatus() const;

    /**
     * Returns the search term in the current quicksearch field.
     */
    [[nodiscard]] QString currentFilterSearchString() const;

    /**
     * Returns true if the current Aggregation is threaded, false otherwise
     * (or if there is no current Aggregation).
     */
    [[nodiscard]] bool isThreaded() const;

    /**
     * Fast function that determines if the selection is empty
     */
    [[nodiscard]] bool selectionEmpty() const;

    /**
     * Fills the lists of the selected message serial numbers and of the selected+visible ones.
     * Returns true if the returned stats are valid (there is a current folder after all)
     * and false otherwise. This is called by KMMainWidget in a single place so we optimize by
     * making it a single sweep on the selection.
     *
     * If includeCollapsedChildren is true then the children of the selected but
     * collapsed items are also included in the stats
     */

    [[nodiscard]] bool getSelectionStats(Akonadi::Item::List &selectedSernums,
                                         Akonadi::Item::List &selectedVisibleSernums,
                                         bool *allSelectedBelongToSameThread,
                                         bool includeCollapsedChildren = true) const;
    /**
     * Deletes the persistent set pointed by the specified reference.
     * If the set does not exist anymore, nothing happens.
     */
    void deletePersistentSet(MessageList::Core::MessageItemSetReference ref);

    /**
     * If bMark is true this function marks the messages as "about to be removed"
     * so they appear dimmer and aren't selectable in the view.
     * If bMark is false then this function clears the "about to be removed" state
     * for the specified MessageItems.
     */
    void markMessageItemsAsAboutToBeRemoved(MessageList::Core::MessageItemSetReference ref, bool bMark);

    /**
     * Return Akonadi::Item from messageItemReference
     */
    [[nodiscard]] Akonadi::Item::List itemListFromPersistentSet(MessageList::Core::MessageItemSetReference ref);

    /**
     * Return a persistent set from current selection
     */
    [[nodiscard]] MessageList::Core::MessageItemSetReference selectionAsPersistentSet(bool includeCollapsedChildren = true) const;

    /**
     * Return a persistent set from current thread
     */
    [[nodiscard]] MessageList::Core::MessageItemSetReference currentThreadAsPersistentSet() const;
    [[nodiscard]] Akonadi::Collection currentCollection() const;

    void setQuickSearchClickMessage(const QString &msg);
    [[nodiscard]] MessageList::Core::SearchMessageByButtons::SearchOptions currentOptions() const;

protected:
    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void fillMessageTagCombo() override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewMessageSelected(MessageList::Core::MessageItem *msg) override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewMessageActivated(MessageList::Core::MessageItem *msg) override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewSelectionChanged() override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewMessageListContextPopupRequest(const QList<Core::MessageItem *> &selectedItems, const QPoint &globalPos) override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewGroupHeaderContextPopupRequest(MessageList::Core::GroupHeaderItem *group, const QPoint &globalPos) override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewDragEnterEvent(QDragEnterEvent *e) override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewDragMoveEvent(QDragMoveEvent *e) override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewDropEvent(QDropEvent *e) override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewStartDragRequest() override;

    /**
     * Reimplemented from MessageList::Core::Widget
     */
    void viewMessageStatusChangeRequest(MessageList::Core::MessageItem *msg, Akonadi::MessageStatus set, Akonadi::MessageStatus clear) override;

private:
    MESSAGELIST_NO_EXPORT void slotCollapseItem();
    MESSAGELIST_NO_EXPORT void slotExpandItem();
    MESSAGELIST_NO_EXPORT void slotTagsFetched(KJob *job);

Q_SIGNALS:
    /**
     * Emitted when a message is selected (that is, single clicked and thus made current in the view)
     * Note that this message CAN be 0 (when the current item is cleared, for example).
     *
     * This signal is emitted when a SINGLE message is selected in the view, probably
     * by clicking on it or by simple keyboard navigation. When multiple items
     * are selected at once (by shift+clicking, for example) then you will get
     * this signal only for the last clicked message (or at all, if the last shift+clicked
     * thing is a group header...). You should handle selection changed in this case.
     */
    void messageSelected(const Akonadi::Item &item);

    /**
     * Emitted when a message is doubleclicked or activated by other input means
     */
    void messageActivated(const Akonadi::Item &item);

    /**
     * Emitted when the selection in the view changes.
     */
    void selectionChanged();

    /**
     * Emitted when a message wants its status to be changed
     */
    void messageStatusChangeRequest(const Akonadi::Item &item, const Akonadi::MessageStatus &set, const Akonadi::MessageStatus &clear);

private:
    class WidgetPrivate;
    std::unique_ptr<WidgetPrivate> const d;
};
} // namespace MessageList
