/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QString>
#include <QWidget>

#include <MessageList/Enums>
#include <MessageList/QuickSearchLine>

class QLineEdit;
class QActionGroup;
class QMenu;

namespace Akonadi
{
class Collection;
class MessageStatus;
}

namespace MessageList
{
namespace Core
{
class GroupHeaderItem;
class MessageItem;
class StorageModel;
class View;
class Filter;
/**
 * Provides a widget which has the messagelist and the most important helper widgets,
 * like the search line and the comboboxes for changing status filtering, aggregation etc.
 */
class Widget : public QWidget
{
    friend class View;

    Q_OBJECT
public:
    explicit Widget(QWidget *parent);
    ~Widget() override;

    /**
     * Sets the storage model for this Widget.
     *
     * Pre-selection is the action of automatically selecting a message just after the folder
     * has finished loading. See Model::setStorageModel() for more information.
     */
    void setStorageModel(StorageModel *storageModel, PreSelectionMode preSelectionMode = PreSelectLastSelected);

    /**
     * Returns the StorageModel currently set. May be nullptr.
     */
    StorageModel *storageModel() const;

    /**
     * Returns the search line of this widget. Can be nullptr if the quick search
     * is disabled in the global configuration.
     */
    QLineEdit *quickSearch() const;

    /**
     * Returns the View attached to this Widget. Never nullptr.
     */
    View *view() const;

    /**
     * Returns the current MessageItem in the current folder.
     * May return nullptr if there is no current message or no current folder.
     */
    Core::MessageItem *currentMessageItem() const;

    /**
     * Returns the Akonadi::MessageStatus in the current quicksearch field.
     */
    Q_REQUIRED_RESULT QVector<Akonadi::MessageStatus> currentFilterStatus() const;

    /**
     * Returns the search term in the current quicksearch field.
     */
    Q_REQUIRED_RESULT QString currentFilterSearchString() const;

    /**
     * Returns the id of the MessageItem::Tag currently set in the quicksearch field.
     */
    Q_REQUIRED_RESULT QString currentFilterTagId() const;

    /**
     * Sets the focus on the quick search line of the currently active tab.
     */
    void focusQuickSearch(const QString &selectedText);

    /**
     * Returns true if the current Aggregation is threaded, false otherwise
     * (or if there is no current Aggregation).
     */
    Q_REQUIRED_RESULT bool isThreaded() const;
    /**
     * Fast function that determines if the selection is empty
     */
    Q_REQUIRED_RESULT bool selectionEmpty() const;

    /**
     * Sets the current folder.
     */
    void setCurrentFolder(const Akonadi::Collection &collection);
    Q_REQUIRED_RESULT Akonadi::Collection currentFolder() const;

    void saveCurrentSelection();

    Q_REQUIRED_RESULT bool searchEditHasFocus() const;

    void sortOrderMenuAboutToShow(QMenu *menu);
    void themeMenuAboutToShow(QMenu *menu);
    void aggregationMenuAboutToShow(QMenu *menu);

    Q_REQUIRED_RESULT MessageList::Core::QuickSearchLine::SearchOptions currentOptions() const;
    void setFilter(Filter *filter);
public Q_SLOTS:

    /**
     * This is called to setup the status filter's QComboBox.
     */
    void populateStatusFilterCombo();

    /**
     * Shows or hides the quicksearch field, the filter combobox and the toolbutton for advanced search.
     */
    void changeQuicksearchVisibility(bool);

    void setLockTab(bool lock);
    Q_REQUIRED_RESULT bool isLocked() const;

protected:
    /**
     * Called when the "Message Status/Tag" filter menu is opened by the user.
     * You may override this function in order to add some "custom tag" entries
     * to the menu. The entries should be placed in a QActionGroup which should be returned
     * to the caller. The QAction objects associated to the entries should have
     * the string id of the tag set as data() and the tag icon set as icon().
     * The default implementation does nothing.
     *
     * Once the tag retrieval is complete call setCurrentStatusFilterItem()
     */
    virtual void fillMessageTagCombo();

    void addMessageTagItem(const QPixmap &, const QString &, const QVariant &);

    /**
     * Must be called by fillMessageTagCombo()
     */
    void setCurrentStatusFilterItem();

    /**
     * This is called by View when a message is single-clicked (thus selected and made current)
     */
    virtual void viewMessageSelected(MessageItem *msg);

    /**
     * This is called by View when a message is double-clicked or activated by other input means
     */
    virtual void viewMessageActivated(MessageItem *msg);

    /**
     * This is called by View when selection changes.
     */
    virtual void viewSelectionChanged();

    /**
     * This is called by View when a message is right clicked.
     */
    virtual void viewMessageListContextPopupRequest(const QVector<MessageItem *> &selectedItems, const QPoint &globalPos);

    /**
     * This is called by View when a group header is right clicked.
     */
    virtual void viewGroupHeaderContextPopupRequest(GroupHeaderItem *group, const QPoint &globalPos);

    /**
     * This is called by View when a drag enter event is received
     */
    virtual void viewDragEnterEvent(QDragEnterEvent *e);

    /**
     * This is called by View when a drag move event is received
     */
    virtual void viewDragMoveEvent(QDragMoveEvent *e);

    /**
     * This is called by View when a drop event is received
     */
    virtual void viewDropEvent(QDropEvent *e);

    /**
     * This is called by View when a drag can possibly be started
     */
    virtual void viewStartDragRequest();

    /**
     * This is called by View when a message item is manipulated by the user
     * in a way that it's status should change. (e.g, by clicking on a status icon, for example).
     */
    virtual void viewMessageStatusChangeRequest(MessageItem *msg, Akonadi::MessageStatus set, Akonadi::MessageStatus clear);

    void tagIdSelected(const QVariant &data);

Q_SIGNALS:
    /**
     * Notify the outside when updating the status bar with a message
     * could be useful
     */
    void statusMessage(const QString &message);
    void forceLostFocus();
    void unlockTabRequested();

protected Q_SLOTS:
    /**
     * This is called by Manager when the option sets stored within have changed.
     */
    void aggregationsChanged();

    /**
     * This is called by Manager when the option sets stored within have changed.
     */
    void themesChanged();

    void themeMenuAboutToShow();
    void aggregationMenuAboutToShow();
    void themeSelected(bool);
    void configureThemes();
    void setPrivateSortOrderForStorage();
    void aggregationSelected(bool);
    void statusSelected(int index);
    void searchEditTextEdited();
    void searchTimerFired();
    void searchEditClearButtonClicked();
    void sortOrderMenuAboutToShow();
    void messageSortingSelected(QAction *action);
    void messageSortDirectionSelected(QAction *action);
    void groupSortingSelected(QAction *action);
    void groupSortDirectionSelected(QAction *action);
    void resetFilter();

    /**
     * Handles header section clicks switching the Aggregation MessageSorting on-the-fly.
     */
    void slotViewHeaderSectionClicked(int logicalIndex);
    void slotStatusButtonsClicked();
    void slotSaveFilter();

private:
    void slotActivateFilter(Filter *f);
    class Private;
    Private *const d;
};
} // namespace Core
} // namespace MessageList

