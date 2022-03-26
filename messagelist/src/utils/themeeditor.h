/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

#include "core/theme.h"
#include "core/themedelegate.h"
#include "utils/optionseteditor.h"

#include <QLabel>
#include <QRect>
#include <QTreeWidget>

#include <QDialog>

class QCheckBox;

class QComboBox;
class KPluralHandlingSpinBox;
class QLineEdit;

namespace MessageList
{
namespace Core
{
class Item;
class GroupHeaderItem;
class MessageItem;
class FakeItem;
class ModelInvariantRowMapper;
} // namespace Core

namespace Utils
{
class ThemeColumnPropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ThemeColumnPropertiesDialog(QWidget *parent, Core::Theme::Column *column, const QString &title);

protected:
    Core::Theme::Column *const mColumn;
    QLineEdit *mNameEdit = nullptr;
    QCheckBox *mVisibleByDefaultCheck = nullptr;
    QCheckBox *mIsSenderOrReceiverCheck = nullptr;
    QComboBox *mMessageSortingCombo = nullptr;

protected Q_SLOTS:
    void slotOkButtonClicked();
};

class ThemePreviewDelegate : public Core::ThemeDelegate
{
    Q_OBJECT
public:
    explicit ThemePreviewDelegate(QAbstractItemView *parent);
    ~ThemePreviewDelegate() override;

private:
    Core::GroupHeaderItem *mSampleGroupHeaderItem = nullptr;
    Core::FakeItem *mSampleMessageItem = nullptr;
    Core::ModelInvariantRowMapper *mRowMapper = nullptr; // needed for the MessageItem above to be valid
public:
    Core::Item *itemFromIndex(const QModelIndex &index) const override;
};

class ThemePreviewWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit ThemePreviewWidget(QWidget *parent);
    ~ThemePreviewWidget() override;
    void setReadOnly(bool readOnly);

private:
    // DnD insert position stuff

    /**
     * The row we'll be inserting the dragged item into
     */
    enum RowInsertPosition {
        AboveRow, ///< We'll insert above the currently hit row in mDelegate
        InsideRow, ///< We'll insert inside the currently hit row in mDelegate
        BelowRow ///< We'll insert below the currently hit row in mDelegate
    };

    /**
     * The position in row that we'll be inserting the dragged item
     */
    enum ItemInsertPosition {
        OnLeftOfItem, ///< We'll insert on the left of the selected item
        OnRightOfItem, ///< We'll insert on the right of the selected item
        AsLastLeftItem, ///< We'll insert as last left item of the row (rightmost left item)
        AsLastRightItem, ///< We'll insert as last right item of the row (leftmost right item)
        AsFirstLeftItem, ///< We'll insert as first left item of the row (leftmost)
        AsFirstRightItem ///< We'll insert as first right item of the row (rightmost)
    };

private:
    ThemePreviewDelegate *mDelegate = nullptr;
    QTreeWidgetItem *mGroupHeaderSampleItem = nullptr;
    QRect mThemeSelectedContentItemRect;
    Core::Theme::ContentItem *mSelectedThemeContentItem = nullptr;
    Core::Theme::Column *mSelectedThemeColumn = nullptr;
    QPoint mMouseDownPoint;
    Core::Theme *mTheme = nullptr;
    RowInsertPosition mRowInsertPosition;
    ItemInsertPosition mItemInsertPosition;
    QPoint mDropIndicatorPoint1;
    QPoint mDropIndicatorPoint2;
    bool mFirstShow;
    bool mReadOnly;

public:
    QSize sizeHint() const override;
    void setTheme(Core::Theme *theme);

protected:
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void paintEvent(QPaintEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void changeEvent(QEvent *event) override;

private:
    void internalHandleDragMoveEvent(QDragMoveEvent *e);
    void internalHandleDragEnterEvent(QDragEnterEvent *e);

    /**
     * Computes the drop insert position for the dragged item at position pos.
     * Returns true if the dragged item can be inserted somewhere and
     * false otherwise. Sets mRowInsertPosition, mItemInsertPosition,
     * mDropIndicatorPoint1 ,mDropIndicatorPoint2.
     */
    bool computeContentItemInsertPosition(const QPoint &pos, Core::Theme::ContentItem::Type type);

    void applyThemeColumnWidths();

protected Q_SLOTS:
    void slotHeaderContextMenuRequested(const QPoint &pos);
    void slotAddColumn();
    void slotColumnProperties();
    void slotDeleteColumn();
    void slotDisabledFlagsMenuTriggered(QAction *act);
    void slotForegroundColorMenuTriggered(QAction *act);
    void slotFontMenuTriggered(QAction *act);
    void slotSoftenActionTriggered(bool);
    void slotGroupHeaderBackgroundModeMenuTriggered(QAction *act);
    void slotGroupHeaderBackgroundStyleMenuTriggered(QAction *act);
    void slotMoveColumnToLeft();
    void slotMoveColumnToRight();
};

class ThemeContentItemSourceLabel : public QLabel
{
    Q_OBJECT
public:
    ThemeContentItemSourceLabel(QWidget *parent, Core::Theme::ContentItem::Type type);
    ~ThemeContentItemSourceLabel() override;

public:
    Core::Theme::ContentItem::Type type() const;
    void startDrag();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

private:
    QPoint mMousePressPoint;
    Core::Theme::ContentItem::Type mType;
};

class ThemeEditor : public OptionSetEditor
{
    Q_OBJECT
public:
    explicit ThemeEditor(QWidget *parent);
    ~ThemeEditor() override;

public:
    /**
     * Sets the option set to be edited.
     * Saves and forgets any previously option set that was being edited.
     * The set parameter may be 0: in this case the editor is simply disabled.
     */
    void editTheme(Core::Theme *set);

    Core::Theme *editedTheme() const;

    void commit();
Q_SIGNALS:
    void themeNameChanged();

private:
    void fillViewHeaderPolicyCombo();

protected Q_SLOTS:
    void slotNameEditTextEdited(const QString &newName) override;
    void slotIconSizeSpinBoxValueChanged(int val);

private:
    void setReadOnly(bool readOnly);

    Core::Theme *mCurrentTheme = nullptr; // shallow, may be null!

    // Appearance tab
    ThemePreviewWidget *mPreviewWidget = nullptr;

    // Advanced tab
    QComboBox *mViewHeaderPolicyCombo = nullptr;
    KPluralHandlingSpinBox *mIconSizeSpinBox = nullptr;
};
} // namespace Utils
} // namespace MessageList
