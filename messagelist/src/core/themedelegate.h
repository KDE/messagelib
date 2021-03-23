/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include <QColor>
#include <QRect>
#include <QStyledItemDelegate>

#include "core/item.h"
#include "core/theme.h"

class QAbstractItemView;

namespace MessageList
{
namespace Core
{
class Item;

/**
 * The ThemeDelegate paints the message list view message and group items by
 * using the supplied Theme.
 */
class ThemeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ThemeDelegate(QAbstractItemView *parent);
    ~ThemeDelegate() override;
    /**
     * Called when the global fonts change (from systemsettings)
     */
    void generalFontChanged();

private:
    const Theme *mTheme = nullptr; ///< Shallow pointer to the current theme
    QAbstractItemView *const mItemView;

    QColor mGroupHeaderBackgroundColor; // cache

    // hitTest results
    QModelIndex mHitIndex;
    Item *mHitItem = nullptr;
    QRect mHitItemRect;
    const Theme::Column *mHitColumn = nullptr;
    const Theme::Row *mHitRow = nullptr;
    int mHitRowIndex;
    bool mHitRowIsMessageRow;
    QRect mHitRowRect;
    bool mHitContentItemRight;
    const Theme::ContentItem *mHitContentItem = nullptr;
    QRect mHitContentItemRect;

    mutable QSize mCachedMessageItemSizeHint;
    mutable QSize mCachedGroupHeaderItemSizeHint;

public:
    const Theme *theme() const;
    void setTheme(const Theme *theme);

    /**
     * Returns a heuristic sizeHint() for the specified item type and column.
     * The hint is based on the contents of the theme (and not of any message or group header).
     */
    QSize sizeHintForItemTypeAndColumn(Item::Type type, int column, const Item *item = nullptr) const;

    /**
     * Performs a hit test on the specified viewport point.
     * Returns true if the point hit something and false otherwise.
     * When the hit test is successful then the hitIndex(), hitItem(), hitColumn(), hitRow(), and hitContentItem()
     * function will return information about the item that was effectively hit.
     * If exact is set to true then hitTest() will return true only if the viewportPoint
     * is exactly over an item. If exact is set to false then the hitTest() function
     * will do its best to find the closest object to be actually "hit": this is useful,
     * for example, in drag and drop operations.
     */
    bool hitTest(const QPoint &viewportPoint, bool exact = true);

    /**
     * Returns the model index that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function.
     */
    const QModelIndex &hitIndex() const;

    /**
     * Returns the Item that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function.
     */
    Item *hitItem() const;

    /**
     * Returns the visual rectangle of the item that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function. Please note that this rectangle refers
     * to a specific item column (and not all of the columns).
     */
    QRect hitItemRect() const;

    /**
     * Returns the theme column that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function.
     */
    const Theme::Column *hitColumn() const;

    /**
     * Returns the index of the theme column that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function.
     * This is the same as hitIndex().column().
     */
    int hitColumnIndex() const;

    /**
     * Returns the theme row that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function. This function may also return a null row
     * when hitTest() returned true. This means that the item was globally hit
     * but no row was exactly hit (the user probably hit the margin instead).
     */
    const Theme::Row *hitRow() const;

    /**
     * Returns the index of the theme row that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitRow() returns a non null value.
     */
    int hitRowIndex() const;

    /**
     * Returns the rectangle of the row that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function. The result of this function is also invalid
     * if hitRow() returns 0.
     */
    QRect hitRowRect() const;

    /**
     * Returns true if the hitRow() is a message row, false otherwise.
     * The result of this function has a meaning only if hitRow() returns a non zero result.
     */
    bool hitRowIsMessageRow() const;

    /**
     * Returns the theme content item that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function. This function may also return a null content item
     * when hitTest() returned true. This means that the item was globally hit
     * but no content item was exactly hit (the user might have clicked inside a blank unused space instead).
     */
    const Theme::ContentItem *hitContentItem() const;

    /**
     * Returns true if the hit theme content item was a right item and false otherwise.
     * The result of this function is valid only if hitContentItem() returns true.
     */
    bool hitContentItemRight() const;

    /**
     * Returns the bounding rect of the content item that was reported as hit by the previous call to hitTest().
     * The result of this function is valid only if hitTest() returned true and only
     * within the same calling function. The result of this function is to be considered
     * invalid also when hitContentItem() returns 0.
     */
    QRect hitContentItemRect() const;

protected:
    /**
     * Returns the Item for the specified model index. Pure virtual: must be reimplemented
     * by derived classes.
     */
    virtual Item *itemFromIndex(const QModelIndex &index) const = 0;

    /**
     * Reimplemented from QStyledItemDelegate
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /**
     * Reimplemented from QStyledItemDelegate
     */
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
} // namespace Core
} // namespace MessageList

