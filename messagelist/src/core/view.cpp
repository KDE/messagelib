/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/view.h"
#include "core/aggregation.h"
#include "core/delegate.h"
#include "core/groupheaderitem.h"
#include "core/item.h"
#include "core/messageitem.h"
#include "core/model.h"
#include "core/storagemodelbase.h"
#include "core/theme.h"
#include "core/widgetbase.h"
#include "messagelistsettings.h"
#include "messagelistutil.h"
#include "messagelistutil_p.h"

#include <MessageCore/StringUtil>

#include <Akonadi/Item>
#include <KTwoFingerTap>
#include <QApplication>
#include <QGestureEvent>
#include <QHeaderView>
#include <QHelpEvent>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QScrollBar>
#include <QScroller>
#include <QTimer>
#include <QToolTip>

#include "messagelist_debug.h"
#include <KLocalizedString>

using namespace MessageList::Core;
using namespace Qt::Literals::StringLiterals;
class View::ViewPrivate
{
public:
    ViewPrivate(View *owner, Widget *parent)
        : q(owner)
        , mWidget(parent)
        , mDelegate(new Delegate(owner))
    {
    }

    void expandFullThread(const QModelIndex &index);
    void generalPaletteChanged();
    void onPressed(QMouseEvent *e);
    void gestureEvent(QGestureEvent *e);
    void tapTriggered(QTapGesture *tap);
    void tapAndHoldTriggered(QTapAndHoldGesture *tap);
    void twoFingerTapTriggered(KTwoFingerTap *tap);

    QColor mTextColor;
    View *const q;

    Widget *const mWidget;
    Model *mModel = nullptr;
    Delegate *const mDelegate;

    const Aggregation *mAggregation = nullptr; ///< The Aggregation we're using now, shallow pointer
    Theme *mTheme = nullptr; ///< The Theme we're using now, shallow pointer
    bool mNeedToApplyThemeColumns = false; ///< Flag signaling a pending application of theme columns
    Item *mLastCurrentItem = nullptr;
    QPoint mMousePressPosition;
    bool mSaveThemeColumnStateOnSectionResize = true; ///< This is used to filter out programmatic column resizes in slotSectionResized().
    QTimer *mSaveThemeColumnStateTimer = nullptr; ///< Used to trigger a delayed "save theme state"
    QTimer *mApplyThemeColumnsTimer = nullptr; ///< Used to trigger a delayed "apply theme columns"
    int mLastViewportWidth = -1;
    bool mIgnoreUpdateGeometries = false; ///< Shall we ignore the "update geometries" calls ?
    QScroller *mScroller = nullptr;
    bool mIsTouchEvent = false;
    bool mMousePressed = false;
    Qt::MouseEventSource mLastMouseSource = Qt::MouseEventNotSynthesized;
    bool mTapAndHoldActive = false;
    QRubberBand *mRubberBand = nullptr;
    Qt::GestureType mTwoFingerTap = Qt::CustomGesture;
};

View::View(Widget *pParent)
    : QTreeView(pParent)
    , d(new ViewPrivate(this, pParent))
{
    d->mSaveThemeColumnStateTimer = new QTimer();
    connect(d->mSaveThemeColumnStateTimer, &QTimer::timeout, this, &View::saveThemeColumnState);

    d->mApplyThemeColumnsTimer = new QTimer();
    connect(d->mApplyThemeColumnsTimer, &QTimer::timeout, this, &View::applyThemeColumns);

    setItemDelegate(d->mDelegate);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setAlternatingRowColors(true);
    setAllColumnsShowFocus(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    viewport()->setAcceptDrops(true);

    d->mScroller = QScroller::scroller(viewport());
    QScrollerProperties scrollerProp;
    scrollerProp.setScrollMetric(QScrollerProperties::AcceleratingFlickMaximumTime, 0.2); // QTBUG-88249
    d->mScroller->setScrollerProperties(scrollerProp);
    d->mScroller->grabGesture(viewport());

    setAttribute(Qt::WA_AcceptTouchEvents);
    d->mTwoFingerTap = QGestureRecognizer::registerRecognizer(new KTwoFingerTapRecognizer());
    viewport()->grabGesture(d->mTwoFingerTap);
    viewport()->grabGesture(Qt::TapGesture);
    viewport()->grabGesture(Qt::TapAndHoldGesture);

    d->mRubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header(), &QWidget::customContextMenuRequested, this, &View::slotHeaderContextMenuRequested);
    connect(header(), &QHeaderView::sectionResized, this, &View::slotHeaderSectionResized);

    header()->setSectionsClickable(true);
    header()->setSectionResizeMode(QHeaderView::Interactive);
    header()->setMinimumSectionSize(2); // QTreeView overrides our sections sizes if we set them smaller than this value
    header()->setDefaultSectionSize(2); // QTreeView overrides our sections sizes if we set them smaller than this value

    d->mModel = new Model(this);
    setModel(d->mModel);

    connect(d->mModel, &Model::statusMessage, pParent, &Widget::statusMessage);

    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &View::slotSelectionChanged, Qt::UniqueConnection);

    // as in KDE3, when a root-item of a message thread is expanded, expand all children
    connect(this, &View::expanded, this, [this](const QModelIndex &index) {
        d->expandFullThread(index);
    });
}

View::~View()
{
    if (d->mSaveThemeColumnStateTimer->isActive()) {
        d->mSaveThemeColumnStateTimer->stop();
    }
    delete d->mSaveThemeColumnStateTimer;
    if (d->mApplyThemeColumnsTimer->isActive()) {
        d->mApplyThemeColumnsTimer->stop();
    }
    delete d->mApplyThemeColumnsTimer;

    // Zero out the theme, aggregation and ApplyThemeColumnsTimer so Model will not cause accesses to them in its destruction process
    d->mApplyThemeColumnsTimer = nullptr;

    d->mTheme = nullptr;
    d->mAggregation = nullptr;
}

Model *View::model() const
{
    return d->mModel;
}

Delegate *View::delegate() const
{
    return d->mDelegate;
}

void View::ignoreCurrentChanges(bool ignore)
{
    if (ignore) {
        disconnect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &View::slotSelectionChanged);
        viewport()->setUpdatesEnabled(false);
    } else {
        connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &View::slotSelectionChanged, Qt::UniqueConnection);
        viewport()->setUpdatesEnabled(true);
    }
}

void View::ignoreUpdateGeometries(bool ignore)
{
    d->mIgnoreUpdateGeometries = ignore;
}

int View::scrollingLockDirection() const
{
    // There is another popular requisite: people want the view to automatically
    // scroll in order to show new arriving mail. This actually makes sense
    // only when the view is sorted by date and the new mail is (usually) either
    // appended at the bottom or inserted at the top. It would be also confusing
    // when the user is browsing some other thread in the meantime.
    //
    // So here we make a simple guess: if the view is scrolled somewhere in the
    // middle then we assume that the user is browsing other threads and we
    // try to keep the currently selected item steady on the screen.
    // When the view is "locked" to the top (scrollbar value 0) or to the
    // bottom (scrollbar value == maximum) then we assume that the user
    // isn't browsing and we should attempt to show the incoming messages
    // by keeping the view "locked".
    //
    // The "locking" also doesn't make sense in the first big fill view job.
    // [Well this concept is pre-akonadi. Now the loading is all async anyway...
    //  So all this code is actually triggered during the initial loading, too.]

    const SortOrder *sortOrder = d->mModel->sortOrder();
    const SortOrder::MessageSorting msgSort = sortOrder->messageSorting();
    const bool sortByDate = //
        msgSort == SortOrder::SortMessagesByDateTime || //
        msgSort == SortOrder::SortMessagesByDateTimeOfMostRecent;

    if (sortByDate) {
        const int scrollBarPosition = verticalScrollBar()->value();
        const int scrollBarMaximum = verticalScrollBar()->maximum();

        const SortOrder::SortDirection sortDirection = sortOrder->messageSortDirection();
        const bool stayAtTop = scrollBarPosition == 0 && sortDirection == SortOrder::Descending;
        const bool stayAtBottom = scrollBarPosition == scrollBarMaximum && sortDirection == SortOrder::Ascending;
        if (stayAtTop) {
            return -1;
        } else if (stayAtBottom) {
            return 1;
        }
    }

    return 0;
}

void View::updateGeometries()
{
    if (d->mIgnoreUpdateGeometries || !d->mModel) {
        return;
    }

    const int scrollLockDirection = scrollingLockDirection();

    QTreeView::updateGeometries();

    if (scrollLockDirection != 0) {
        // we prefer to keep the view locked to the top or bottom
        if (scrollLockDirection == 1) {
            // we wanted the view to be locked to the bottom
            if (verticalScrollBar()->value() != verticalScrollBar()->maximum()) {
                verticalScrollBar()->setValue(verticalScrollBar()->maximum());
            }
        } // else we wanted the view to be locked to top and we shouldn't need to do anything
    }
}

StorageModel *View::storageModel() const
{
    return d->mModel->storageModel();
}

void View::setAggregation(const Aggregation *aggregation)
{
    d->mAggregation = aggregation;
    d->mModel->setAggregation(aggregation);

    // use uniform row heights to speed up, but only if there are no group headers used
    setUniformRowHeights(d->mAggregation->grouping() == Aggregation::NoGrouping);
}

void View::setTheme(Theme *theme)
{
    d->mNeedToApplyThemeColumns = true;
    d->mTheme = theme;
    d->mDelegate->setTheme(theme);
    d->mModel->setTheme(theme);
}

void View::setSortOrder(const SortOrder *sortOrder)
{
    d->mModel->setSortOrder(sortOrder);
}

void View::reload()
{
    setStorageModel(storageModel());
}

void View::setStorageModel(StorageModel *storageModel, PreSelectionMode preSelectionMode)
{
    // This will cause the model to be reset.
    d->mSaveThemeColumnStateOnSectionResize = false;
    d->mModel->setStorageModel(storageModel, preSelectionMode);
    d->mSaveThemeColumnStateOnSectionResize = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Theme column state machinery
//
// This is yet another beast to beat. The QHeaderView behaviour, at the time of writing,
// is quite unpredictable. This is due to the complex interaction with the model, with the QTreeView
// and due to its attempts to delay the layout jobs. The delayed layouts, especially, may
// cause the widths of the columns to quickly change in an unexpected manner in a place
// where previously they have been always settled to the values you set...
//
// So here we have the tools to:
//
// - Apply the saved state of the theme columns (applyThemeColumns()).
//   This function computes the "best fit" state of the visible columns and tries
//   to apply it to QHeaderView. It also saves the new computed state to the Theme object.
//
// - Explicitly save the column state, used when the user changes the widths or visibility manually.
//   This is called through a delayed timer after a column has been resized or used directly
//   when the visibility state of a column has been changed by toggling a popup menu entry.
//
// - Display the column state context popup menu and handle its actions
//
// - Apply the theme columns when the theme changes, when the model changes or when
//   the widget is resized.
//
// - Avoid saving a corrupted column state in that QHeaderView can be found *very* frequently.
//

void View::applyThemeColumns()
{
    if (!d->mApplyThemeColumnsTimer) {
        return;
    }

    if (d->mApplyThemeColumnsTimer->isActive()) {
        d->mApplyThemeColumnsTimer->stop();
    }

    if (!d->mTheme) {
        return;
    }

    // qCDebug(MESSAGELIST_LOG) << "Apply theme columns";

    const QList<Theme::Column *> &columns = d->mTheme->columns();

    if (columns.isEmpty()) {
        return; // bad theme
    }

    if (!viewport()->isVisible()) {
        return; // invisible
    }

    if (viewport()->width() < 1) {
        return; // insane width
    }
    const int viewportWidth = viewport()->width();
    d->mLastViewportWidth = viewportWidth;

    // Now we want to distribute the available width on all the visible columns.
    //
    // The rules:
    // - The visible columns will span the width of the view, if possible.
    // - The columns with a saved width should take that width.
    // - The columns on the left should take more space, if possible.
    // - The columns with no text take just slightly more than their size hint.
    //   while the columns with text take possibly a lot more.
    //

    // Note that the first column is always shown (it can't be hidden at all)

    // The algorithm below is a sort of compromise between:
    // - Saving the user preferences for widths
    // - Using exactly the available view space
    //
    // It "tends to work" in all cases:
    // - When there are no user preferences saved and the column widths must be
    //   automatically computed to make best use of available space
    // - When there are user preferences for only some of the columns
    //   and that should be somewhat preserved while still using all the
    //   available space.
    // - When all the columns have well defined saved widths

    int idx = 0;

    // Gather total size "hint" for visible sections: if the widths of the columns wers
    // all saved then the total hint is equal to the total saved width.

    int totalVisibleWidthHint = 0;
    QList<int> lColumnSizeHints;
    for (const auto col : std::as_const(columns)) {
        if (col->currentlyVisible() || (idx == 0)) {
            // qCDebug(MESSAGELIST_LOG) << "Column " << idx << " will be visible";
            // Column visible
            const int savedWidth = col->currentWidth();
            const int hintWidth = d->mDelegate->sizeHintForItemTypeAndColumn(Item::Message, idx).width();
            totalVisibleWidthHint += savedWidth > 0 ? savedWidth : hintWidth;
            lColumnSizeHints.append(hintWidth);
            // qCDebug(MESSAGELIST_LOG) << "Column " << idx << " size hint is " << hintWidth;
        } else {
            // qCDebug(MESSAGELIST_LOG) << "Column " << idx << " will be not visible";
            // The column is not visible
            lColumnSizeHints.append(-1); // dummy
        }
        idx++;
    }

    if (totalVisibleWidthHint < 16) {
        totalVisibleWidthHint = 16; // be reasonable
    }

    // Now compute somewhat "proportional" widths.
    idx = 0;

    QList<double> lColumnWidths;
    lColumnWidths.reserve(columns.count());
    int totalVisibleWidth = 0;
    for (const auto col : std::as_const(columns)) {
        double savedWidth = col->currentWidth();
        double hintWidth = savedWidth > 0 ? savedWidth : lColumnSizeHints.at(idx);
        double realWidth;

        if (col->currentlyVisible() || (idx == 0)) {
            if (col->containsTextItems()) {
                // the column contains text items, it should get more space (if possible)
                realWidth = ((hintWidth * viewportWidth) / totalVisibleWidthHint);
            } else {
                // the column contains no text items, it should get exactly its hint/saved width.
                realWidth = hintWidth;
            }

            if (realWidth < 2) {
                realWidth = 2; // don't allow very insane values
            }

            totalVisibleWidth += realWidth;
        } else {
            // Column not visible
            realWidth = -1;
        }

        lColumnWidths.append(realWidth);

        idx++;
    }

    // Now the algorithm above may be wrong for several reasons...
    // - We're using fixed widths for certain columns and proportional
    //   for others...
    // - The user might have changed the width of the view from the
    //   time in that the widths have been saved
    // - There are some (not well identified) issues with the QTreeView
    //   scrollbar that make our view appear larger or shorter by 2-3 pixels
    //   sometimes.
    // - ...
    // So we correct the previous estimates by trying to use exactly
    // the available space.

    idx = 0;

    if (totalVisibleWidth != viewportWidth) {
        // The estimated widths were not using exactly the available space.
        if (totalVisibleWidth < viewportWidth) {
            // We were using less space than available.

            // Give the additional space to the text columns
            // also give more space to the first ones and less space to the last ones
            qreal available = viewportWidth - totalVisibleWidth;

            for (int idx = 0; idx < columns.count(); ++idx) {
                Theme::Column *column = columns.at(idx);
                if ((column->currentlyVisible() || (idx == 0)) && column->containsTextItems()) {
                    // give more space to this column
                    available /= 2; // eat half of the available space
                    lColumnWidths[idx] += available; // and give it to this column
                    if (available < 1) {
                        break; // no more space to give away
                    }
                }
            }

            // if any space is still available, give it to the first column
            if (available >= 1) {
                lColumnWidths[0] += available;
            }
        } else {
            // We were using more space than available

            // If the columns span more than the view then
            // try to squeeze them in order to make them fit
            double missing = totalVisibleWidth - viewportWidth;
            if (missing > 0) {
                const int count = lColumnWidths.count();
                idx = count - 1;

                while (idx >= 0) {
                    if (columns.at(idx)->currentlyVisible() || (idx == 0)) {
                        double chop = lColumnWidths.at(idx) - lColumnSizeHints.at(idx);
                        if (chop > 0) {
                            if (chop > missing) {
                                chop = missing;
                            }
                            lColumnWidths[idx] -= chop;
                            missing -= chop;
                            if (missing < 1) {
                                break; // no more space to recover
                            }
                        }
                    } // else it's invisible
                    idx--;
                }
            }
        }
    }

    // We're ready to assign widths.

    bool oldSave = d->mSaveThemeColumnStateOnSectionResize;
    d->mSaveThemeColumnStateOnSectionResize = false;

    // A huge problem here is that QHeaderView goes quite nuts if we show or hide sections
    // while resizing them. This is because it has several machineries aimed to delay
    // the layout to the last possible moment. So if we show a column, it will tend to
    // screw up the layout of other ones.

    // We first loop showing/hiding columns then.

    idx = 0;

    for (const auto col : std::as_const(columns)) {
        bool visible = (idx == 0) || col->currentlyVisible();
        // qCDebug(MESSAGELIST_LOG) << "Column " << idx << " visible " << visible;
        col->setCurrentlyVisible(visible);
        header()->setSectionHidden(idx, !visible);
        idx++;
    }

    // Then we loop assigning widths. This is still complicated since QHeaderView tries
    // very badly to stretch the last section and thus will resize it in the meantime.
    // But seems to work most of the times...

    idx = 0;

    for (const auto col : std::as_const(columns)) {
        if (col->currentlyVisible()) {
            const double columnWidth(lColumnWidths.at(idx));
            col->setCurrentWidth(columnWidth);
            // Laurent Bug 358855 - message list column widths lost when program closed
            // I need to investigate if this code is still necessary (all method)
            header()->resizeSection(idx, static_cast<int>(columnWidth));
        } else {
            col->setCurrentWidth(-1);
        }
        idx++;
    }

    idx = 0;

    bool bTriggeredQtBug = false;
    for (const auto col : std::as_const(columns)) {
        if (!header()->isSectionHidden(idx)) {
            if (!col->currentlyVisible()) {
                bTriggeredQtBug = true;
            }
        }
        idx++;
    }

    setHeaderHidden(d->mTheme->viewHeaderPolicy() == Theme::NeverShowHeader);

    d->mSaveThemeColumnStateOnSectionResize = oldSave;
    d->mNeedToApplyThemeColumns = false;

    static bool bAllowRecursion = true;

    if (bTriggeredQtBug && bAllowRecursion) {
        bAllowRecursion = false;
        // qCDebug(MESSAGELIST_LOG) << "I've triggered the QHeaderView bug: trying to fix by calling myself again";
        applyThemeColumns();
        bAllowRecursion = true;
    }
}

void View::triggerDelayedApplyThemeColumns()
{
    if (d->mApplyThemeColumnsTimer->isActive()) {
        d->mApplyThemeColumnsTimer->stop();
    }
    d->mApplyThemeColumnsTimer->setSingleShot(true);
    d->mApplyThemeColumnsTimer->start(100);
}

void View::saveThemeColumnState()
{
    if (d->mSaveThemeColumnStateTimer->isActive()) {
        d->mSaveThemeColumnStateTimer->stop();
    }

    if (!d->mTheme) {
        return;
    }

    if (d->mNeedToApplyThemeColumns) {
        return; // don't save the state if it hasn't been applied at all
    }

    // qCDebug(MESSAGELIST_LOG) << "Save theme column state";

    const auto columns = d->mTheme->columns();

    if (columns.isEmpty()) {
        return; // bad theme
    }

    int idx = 0;

    for (const auto col : std::as_const(columns)) {
        if (header()->isSectionHidden(idx)) {
            // qCDebug(MESSAGELIST_LOG) << "Section " << idx << " is hidden";
            col->setCurrentlyVisible(false);
            col->setCurrentWidth(-1); // reset (hmmm... we could use the "don't touch" policy here too...)
        } else {
            // qCDebug(MESSAGELIST_LOG) << "Section " << idx << " is visible and has size " << header()->sectionSize( idx );
            col->setCurrentlyVisible(true);
            col->setCurrentWidth(header()->sectionSize(idx));
        }
        idx++;
    }
}

void View::triggerDelayedSaveThemeColumnState()
{
    if (d->mSaveThemeColumnStateTimer->isActive()) {
        d->mSaveThemeColumnStateTimer->stop();
    }
    d->mSaveThemeColumnStateTimer->setSingleShot(true);
    d->mSaveThemeColumnStateTimer->start(200);
}

void View::resizeEvent(QResizeEvent *e)
{
    qCDebug(MESSAGELIST_LOG) << "Resize event enter (viewport width is " << viewport()->width() << ")";

    QTreeView::resizeEvent(e);

    if (!isVisible()) {
        return; // don't play with
    }

    if (d->mLastViewportWidth != viewport()->width()) {
        triggerDelayedApplyThemeColumns();
    }

    if (header()->isVisible()) {
        return;
    }

    // header invisible

    bool oldSave = d->mSaveThemeColumnStateOnSectionResize;
    d->mSaveThemeColumnStateOnSectionResize = false;

    const int count = header()->count();
    if ((count - header()->hiddenSectionCount()) < 2) {
        // a single column visible: resize it
        int visibleIndex;
        for (visibleIndex = 0; visibleIndex < count; visibleIndex++) {
            if (!header()->isSectionHidden(visibleIndex)) {
                break;
            }
        }
        if (visibleIndex < count) {
            header()->resizeSection(visibleIndex, viewport()->width() - 4);
        }
    }

    d->mSaveThemeColumnStateOnSectionResize = oldSave;

    triggerDelayedSaveThemeColumnState();
}

void View::paintEvent(QPaintEvent *event)
{
#if 0
    if (/*mFirstResult &&*/ (!model() || model()->rowCount() == 0)) {
        QPainter p(viewport());

        QFont font = p.font();
        font.setItalic(true);
        p.setFont(font);

        if (!d->mTextColor.isValid()) {
            d->generalPaletteChanged();
        }
        p.setPen(d->mTextColor);

        p.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, i18n("No result found"));
    } else {
        QTreeView::paintEvent(event);
    }
#else
    QTreeView::paintEvent(event);
#endif
}

void View::modelAboutToEmitLayoutChanged()
{
    // QHeaderView goes totally NUTS with a layoutChanged() call
    d->mSaveThemeColumnStateOnSectionResize = false;
}

void View::modelEmittedLayoutChanged()
{
    // This is after a first chunk of work has been done by the model: do apply column states
    d->mSaveThemeColumnStateOnSectionResize = true;
    applyThemeColumns();
}

void View::slotHeaderSectionResized(int logicalIndex, int oldWidth, int newWidth)
{
    Q_UNUSED(logicalIndex)
    Q_UNUSED(oldWidth)
    Q_UNUSED(newWidth)

    if (d->mSaveThemeColumnStateOnSectionResize) {
        triggerDelayedSaveThemeColumnState();
    }
}

int View::sizeHintForColumn(int logicalColumnIndex) const
{
    // QTreeView: please don't touch my column widths...
    int w = header()->sectionSize(logicalColumnIndex);
    if (w > 0) {
        return w;
    }
    if (!d->mDelegate) {
        return 32; // dummy
    }
    w = d->mDelegate->sizeHintForItemTypeAndColumn(Item::Message, logicalColumnIndex).width();
    return w;
}

void View::slotHeaderContextMenuRequested(const QPoint &pnt)
{
    if (!d->mTheme) {
        return;
    }

    const auto columns = d->mTheme->columns();

    if (columns.isEmpty()) {
        return; // bad theme
    }

    // the menu for the columns
    QMenu menu;

    int idx = 0;
    for (const auto col : std::as_const(columns)) {
        QAction *act = menu.addAction(col->label());
        act->setCheckable(true);
        act->setChecked(!header()->isSectionHidden(idx));
        if (idx == 0) {
            act->setEnabled(false);
        }
        QObject::connect(act, &QAction::triggered, this, [this, idx] {
            slotShowHideColumn(idx);
        });

        idx++;
    }

    menu.addSeparator();
    {
        QAction *act = menu.addAction(i18n("Adjust Column Sizes"));
        QObject::connect(act, &QAction::triggered, this, &View::slotAdjustColumnSizes);
    }
    {
        QAction *act = menu.addAction(i18n("Show Default Columns"));
        QObject::connect(act, &QAction::triggered, this, &View::slotShowDefaultColumns);
    }
    menu.addSeparator();
    {
        QAction *act = menu.addAction(i18n("Display Tooltips"));
        act->setCheckable(true);
        act->setChecked(MessageListSettings::self()->messageToolTipEnabled());
        QObject::connect(act, &QAction::triggered, this, &View::slotDisplayTooltips);
    }
    menu.addSeparator();

    MessageList::Util::fillViewMenu(&menu, d->mWidget);

    menu.exec(header()->mapToGlobal(pnt));
}

void View::slotAdjustColumnSizes()
{
    if (!d->mTheme) {
        return;
    }

    d->mTheme->resetColumnSizes();
    applyThemeColumns();
}

void View::slotShowDefaultColumns()
{
    if (!d->mTheme) {
        return;
    }

    d->mTheme->resetColumnState();
    applyThemeColumns();
}

void View::slotDisplayTooltips(bool showTooltips)
{
    MessageListSettings::self()->setMessageToolTipEnabled(showTooltips);
}

void View::slotShowHideColumn(int columnIdx)
{
    if (!d->mTheme) {
        return; // oops
    }

    if (columnIdx == 0) {
        return; // can never be hidden
    }

    if (columnIdx >= d->mTheme->columns().count()) {
        return;
    }

    const bool showIt = header()->isSectionHidden(columnIdx);

    Theme::Column *column = d->mTheme->columns().at(columnIdx);
    Q_ASSERT(column);

    // first save column state (as it is, with the column still in previous state)
    saveThemeColumnState();

    // If a section has just been shown, invalidate its width in the skin
    // since QTreeView assigned it a (possibly insane) default width.
    // If a section has been hidden, then invalidate its width anyway...
    // so finally invalidate width always, here.
    column->setCurrentlyVisible(showIt);
    column->setCurrentWidth(-1);

    // then apply theme columns to re-compute proportional widths (so we hopefully stay in the view)
    applyThemeColumns();
}

Item *View::currentItem() const
{
    QModelIndex idx = currentIndex();
    if (!idx.isValid()) {
        return nullptr;
    }
    Item *it = static_cast<Item *>(idx.internalPointer());
    Q_ASSERT(it);
    return it;
}

MessageItem *View::currentMessageItem(bool selectIfNeeded) const
{
    Item *it = currentItem();
    if (!it || (it->type() != Item::Message)) {
        return nullptr;
    }

    if (selectIfNeeded) {
        // Keep things coherent, if the user didn't select it, but acted on it via
        // a shortcut, do select it now.
        if (!selectionModel()->isSelected(currentIndex())) {
            selectionModel()->select(currentIndex(), QItemSelectionModel::Select | QItemSelectionModel::Current | QItemSelectionModel::Rows);
        }
    }

    return static_cast<MessageItem *>(it);
}

void View::setCurrentMessageItem(MessageItem *it, bool center)
{
    if (it) {
        qCDebug(MESSAGELIST_LOG) << "Setting current message to" << it->subject();

        const QModelIndex index = d->mModel->index(it, 0);
        selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select | QItemSelectionModel::Current | QItemSelectionModel::Rows);
        if (center) {
            scrollTo(index, QAbstractItemView::PositionAtCenter);
        }
    } else {
        selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Current | QItemSelectionModel::Clear);
    }
}

bool View::selectionEmpty() const
{
    return selectionModel()->selectedRows().isEmpty();
}

QList<MessageItem *> View::selectionAsMessageItemList(bool includeCollapsedChildren) const
{
    QList<MessageItem *> selectedMessages;

    QModelIndexList lSelected = selectionModel()->selectedRows();
    if (lSelected.isEmpty()) {
        return selectedMessages;
    }
    for (const auto &idx : std::as_const(lSelected)) {
        // The asserts below are theoretically valid but at the time
        // of writing they fail because of a bug in QItemSelectionModel::selectedRows()
        // which returns also non-selectable items.

        // Q_ASSERT( selectedItem->type() == Item::Message );
        // Q_ASSERT( ( *it ).isValid() );

        if (!idx.isValid()) {
            continue;
        }

        Item *selectedItem = static_cast<Item *>(idx.internalPointer());
        Q_ASSERT(selectedItem);

        if (selectedItem->type() != Item::Message) {
            continue;
        }

        if (!static_cast<MessageItem *>(selectedItem)->isValid()) {
            continue;
        }

        Q_ASSERT(!selectedMessages.contains(static_cast<MessageItem *>(selectedItem)));

        if (includeCollapsedChildren && (selectedItem->childItemCount() > 0) && (!isExpanded(idx))) {
            static_cast<MessageItem *>(selectedItem)->subTreeToList(selectedMessages);
        } else {
            selectedMessages.append(static_cast<MessageItem *>(selectedItem));
        }
    }

    return selectedMessages;
}

QList<MessageItem *> View::currentThreadAsMessageItemList() const
{
    QList<MessageItem *> currentThread;

    MessageItem *msg = currentMessageItem();
    if (!msg) {
        return currentThread;
    }

    while (msg->parent()) {
        if (msg->parent()->type() != Item::Message) {
            break;
        }
        msg = static_cast<MessageItem *>(msg->parent());
    }

    msg->subTreeToList(currentThread);

    return currentThread;
}

void View::setChildrenExpanded(const Item *root, bool expand)
{
    Q_ASSERT(root);
    auto childList = root->childItems();
    if (!childList) {
        return;
    }
    for (const auto child : std::as_const(*childList)) {
        QModelIndex idx = d->mModel->index(child, 0);
        Q_ASSERT(idx.isValid());
        Q_ASSERT(static_cast<Item *>(idx.internalPointer()) == child);

        if (expand) {
            setExpanded(idx, true);

            if (child->childItemCount() > 0) {
                setChildrenExpanded(child, true);
            }
        } else {
            if (child->childItemCount() > 0) {
                setChildrenExpanded(child, false);
            }

            setExpanded(idx, false);
        }
    }
}

void View::ViewPrivate::generalPaletteChanged()
{
    const QPalette palette = q->viewport()->palette();
    QColor color = palette.text().color();
    color.setAlpha(128);
    mTextColor = color;
}

void View::ViewPrivate::expandFullThread(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    Item *item = static_cast<Item *>(index.internalPointer());
    if (item->type() != Item::Message) {
        return;
    }

    if (!static_cast<MessageItem *>(item)->parent() || (static_cast<MessageItem *>(item)->parent()->type() != Item::Message)) {
        q->setChildrenExpanded(item, true);
    }
}

void View::setCurrentThreadExpanded(bool expand)
{
    Item *it = currentItem();
    if (!it) {
        return;
    }

    if (it->type() == Item::GroupHeader) {
        setExpanded(currentIndex(), expand);
    } else if (it->type() == Item::Message) {
        auto message = static_cast<MessageItem *>(it);
        while (message->parent()) {
            if (message->parent()->type() != Item::Message) {
                break;
            }
            message = static_cast<MessageItem *>(message->parent());
        }

        if (expand) {
            setExpanded(d->mModel->index(message, 0), true);
            setChildrenExpanded(message, true);
        } else {
            setChildrenExpanded(message, false);
            setExpanded(d->mModel->index(message, 0), false);
        }
    }
}

void View::setAllThreadsExpanded(bool expand)
{
    scheduleDelayedItemsLayout();
    if (d->mAggregation->grouping() == Aggregation::NoGrouping) {
        // we have no groups so threads start under the root item: just expand/unexpand all
        setChildrenExpanded(d->mModel->rootItem(), expand);
        return;
    }

    // grouping is in effect: must expand/unexpand one level lower

    auto childList = d->mModel->rootItem()->childItems();
    if (!childList) {
        return;
    }

    for (const auto item : std::as_const(*childList)) {
        setChildrenExpanded(item, expand);
    }
}

void View::setAllGroupsExpanded(bool expand)
{
    if (d->mAggregation->grouping() == Aggregation::NoGrouping) {
        return; // no grouping in effect
    }

    Item *item = d->mModel->rootItem();

    auto childList = item->childItems();
    if (!childList) {
        return;
    }

    scheduleDelayedItemsLayout();
    for (const auto item : std::as_const(*childList)) {
        Q_ASSERT(item->type() == Item::GroupHeader);
        QModelIndex idx = d->mModel->index(item, 0);
        Q_ASSERT(idx.isValid());
        Q_ASSERT(static_cast<Item *>(idx.internalPointer()) == item);
        if (expand) {
            if (!isExpanded(idx)) {
                setExpanded(idx, true);
            }
        } else {
            if (isExpanded(idx)) {
                setExpanded(idx, false);
            }
        }
    }
}

void View::selectMessageItems(const QList<MessageItem *> &list)
{
    QItemSelection selection;
    for (const auto mi : list) {
        Q_ASSERT(mi);
        QModelIndex idx = d->mModel->index(mi, 0);
        Q_ASSERT(idx.isValid());
        Q_ASSERT(static_cast<MessageItem *>(idx.internalPointer()) == mi);
        if (!selectionModel()->isSelected(idx)) {
            selection.append(QItemSelectionRange(idx));
        }
        ensureDisplayedWithParentsExpanded(mi);
    }
    if (!selection.isEmpty()) {
        selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

static inline bool message_type_matches(Item *item, MessageTypeFilter messageTypeFilter)
{
    switch (messageTypeFilter) {
    case MessageTypeAny:
        return true;
        break;
    case MessageTypeUnreadOnly:
        return !item->status().isRead();
        break;
    default:
        // nothing here
        break;
    }

    // never reached
    Q_ASSERT(false);
    return false;
}

Item *View::messageItemAfter(Item *referenceItem, MessageTypeFilter messageTypeFilter, bool loop)
{
    if (!storageModel()) {
        return nullptr; // no folder
    }

    // find the item to start with
    Item *below;

    if (referenceItem) {
        // there was a current item: we start just below it
        if ((referenceItem->childItemCount() > 0) && ((messageTypeFilter != MessageTypeAny) || isExpanded(d->mModel->index(referenceItem, 0)))) {
            // the current item had children: either expanded or we want unread/new messages (and so we'll expand it if it isn't)
            below = referenceItem->itemBelow();
        } else {
            // the current item had no children: ask the parent to find the item below
            Q_ASSERT(referenceItem->parent());
            below = referenceItem->parent()->itemBelowChild(referenceItem);
        }

        if (!below) {
            // reached the end
            if (loop) {
                // try re-starting from top
                below = d->mModel->rootItem()->itemBelow();
                Q_ASSERT(below); // must exist (we had a current item)

                if (below == referenceItem) {
                    return nullptr; // only one item in folder: loop complete
                }
            } else {
                // looping not requested
                return nullptr;
            }
        }
    } else {
        // there was no current item, start from beginning
        below = d->mModel->rootItem()->itemBelow();

        if (!below) {
            return nullptr; // folder empty
        }
    }

    // ok.. now below points to the next message.
    // While it doesn't satisfy our requirements, go further down

    QModelIndex parentIndex = d->mModel->index(below->parent(), 0);
    QModelIndex belowIndex = d->mModel->index(below, 0);

    Q_ASSERT(belowIndex.isValid());

    while (
        // is not a message (we want messages, don't we ?)
        (below->type() != Item::Message) || // message filter doesn't match
        (!message_type_matches(below, messageTypeFilter)) || // is hidden (and we don't want hidden items as they aren't "officially" in the view)
        isRowHidden(belowIndex.row(), parentIndex) || // is not enabled or not selectable
        ((d->mModel->flags(belowIndex) & (Qt::ItemIsSelectable | Qt::ItemIsEnabled)) != (Qt::ItemIsSelectable | Qt::ItemIsEnabled))) {
        // find the next one
        if ((below->childItemCount() > 0) && ((messageTypeFilter != MessageTypeAny) || isExpanded(belowIndex))) {
            // the current item had children: either expanded or we want unread messages (and so we'll expand it if it isn't)
            below = below->itemBelow();
        } else {
            // the current item had no children: ask the parent to find the item below
            Q_ASSERT(below->parent());
            below = below->parent()->itemBelowChild(below);
        }

        if (!below) {
            // we reached the end of the folder
            if (loop) {
                // looping requested
                if (referenceItem) { // <-- this means "we have started from something that is not the top: looping makes sense"
                    below = d->mModel->rootItem()->itemBelow();
                }
                // else mi == 0 and below == 0: we have started from the beginning and reached the end (it will fail the test below and exit)
            } else {
                // looping not requested: nothing more to do
                return nullptr;
            }
        }

        if (below == referenceItem) {
            Q_ASSERT(loop);
            return nullptr; // looped and returned back to the first message
        }

        parentIndex = d->mModel->index(below->parent(), 0);
        belowIndex = d->mModel->index(below, 0);

        Q_ASSERT(belowIndex.isValid());
    }

    return below;
}

Item *View::firstMessageItem(MessageTypeFilter messageTypeFilter)
{
    return messageItemAfter(nullptr, messageTypeFilter, false);
}

Item *View::nextMessageItem(MessageTypeFilter messageTypeFilter, bool loop)
{
    return messageItemAfter(currentMessageItem(false), messageTypeFilter, loop);
}

Item *View::deepestExpandedChild(Item *referenceItem) const
{
    const int children = referenceItem->childItemCount();
    if (children > 0 && isExpanded(d->mModel->index(referenceItem, 0))) {
        return deepestExpandedChild(referenceItem->childItem(children - 1));
    } else {
        return referenceItem;
    }
}

Item *View::messageItemBefore(Item *referenceItem, MessageTypeFilter messageTypeFilter, bool loop)
{
    if (!storageModel()) {
        return nullptr; // no folder
    }

    // find the item to start with
    Item *above;

    if (referenceItem) {
        Item *parent = referenceItem->parent();
        Item *siblingAbove = parent ? parent->itemAboveChild(referenceItem) : nullptr;
        // there was a current item: we start just above it
        if ((siblingAbove && siblingAbove != referenceItem && siblingAbove != parent) && (siblingAbove->childItemCount() > 0)
            && ((messageTypeFilter != MessageTypeAny) || (isExpanded(d->mModel->index(siblingAbove, 0))))) {
            // the current item had children: either expanded or we want unread/new messages (and so we'll expand it if it isn't)
            above = deepestExpandedChild(siblingAbove);
        } else {
            // the current item had no children: ask the parent to find the item above
            Q_ASSERT(referenceItem->parent());
            above = referenceItem->parent()->itemAboveChild(referenceItem);
        }

        if ((!above) || (above == d->mModel->rootItem())) {
            // reached the beginning
            if (loop) {
                // try re-starting from bottom
                above = d->mModel->rootItem()->deepestItem();
                Q_ASSERT(above); // must exist (we had a current item)
                Q_ASSERT(above != d->mModel->rootItem());

                if (above == referenceItem) {
                    return nullptr; // only one item in folder: loop complete
                }
            } else {
                // looping not requested
                return nullptr;
            }
        }
    } else {
        // there was no current item, start from end
        above = d->mModel->rootItem()->deepestItem();

        if (!above || !above->parent() || (above == d->mModel->rootItem())) {
            return nullptr; // folder empty
        }
    }

    // ok.. now below points to the previous message.
    // While it doesn't satisfy our requirements, go further up

    QModelIndex parentIndex = d->mModel->index(above->parent(), 0);
    QModelIndex aboveIndex = d->mModel->index(above, 0);

    Q_ASSERT(aboveIndex.isValid());

    while (
        // is not a message (we want messages, don't we ?)
        (above->type() != Item::Message) || // message filter doesn't match
        (!message_type_matches(above, messageTypeFilter)) || // we don't expand items but the item has parents unexpanded (so should be skipped)
        (
            // !expand items
            (messageTypeFilter == MessageTypeAny) && // has unexpanded parents or is itself hidden
            (!isDisplayedWithParentsExpanded(above)))
        || // is hidden
        isRowHidden(aboveIndex.row(), parentIndex) || // is not enabled or not selectable
        ((d->mModel->flags(aboveIndex) & (Qt::ItemIsSelectable | Qt::ItemIsEnabled)) != (Qt::ItemIsSelectable | Qt::ItemIsEnabled))) {
        above = above->itemAbove();

        if ((!above) || (above == d->mModel->rootItem())) {
            // reached the beginning
            if (loop) {
                // looping requested
                if (referenceItem) { // <-- this means "we have started from something that is not the beginning: looping makes sense"
                    above = d->mModel->rootItem()->deepestItem();
                }
                // else mi == 0 and above == 0: we have started from the end and reached the beginning (it will fail the test below and exit)
            } else {
                // looping not requested: nothing more to do
                return nullptr;
            }
        }

        if (above == referenceItem) {
            Q_ASSERT(loop);
            return nullptr; // looped and returned back to the first message
        }

        if (!above->parent()) {
            return nullptr;
        }

        parentIndex = d->mModel->index(above->parent(), 0);
        aboveIndex = d->mModel->index(above, 0);

        Q_ASSERT(aboveIndex.isValid());
    }

    return above;
}

Item *View::lastMessageItem(MessageTypeFilter messageTypeFilter)
{
    return messageItemBefore(nullptr, messageTypeFilter, false);
}

Item *View::previousMessageItem(MessageTypeFilter messageTypeFilter, bool loop)
{
    return messageItemBefore(currentMessageItem(false), messageTypeFilter, loop);
}

void View::growOrShrinkExistingSelection(const QModelIndex &newSelectedIndex, bool movingUp)
{
    // Qt: why visualIndex() is private? ...I'd really need it here...

    int selectedVisualCoordinate = visualRect(newSelectedIndex).top();

    int topVisualCoordinate = 0xfffffff; // huuuuuge number
    int bottomVisualCoordinate = -(0xfffffff);

    QModelIndex bottomIndex;
    QModelIndex topIndex;

    // find out the actual selection range
    const QItemSelection selection = selectionModel()->selection();

    for (const QItemSelectionRange &range : selection) {
        // We're asking the model for the index as range.topLeft() and range.bottomRight()
        // can return indexes in invisible columns which have a null visualRect().
        // Column 0, instead, is always visible.

        QModelIndex top = d->mModel->index(range.top(), 0, range.parent());
        QModelIndex bottom = d->mModel->index(range.bottom(), 0, range.parent());

        if (top.isValid()) {
            if (!bottom.isValid()) {
                bottom = top;
            }
        } else {
            if (!top.isValid()) {
                top = bottom;
            }
        }
        int candidate = visualRect(bottom).bottom();
        if (candidate > bottomVisualCoordinate) {
            bottomVisualCoordinate = candidate;
            bottomIndex = range.bottomRight();
        }

        candidate = visualRect(top).top();
        if (candidate < topVisualCoordinate) {
            topVisualCoordinate = candidate;
            topIndex = range.topLeft();
        }
    }

    if (topIndex.isValid() && bottomIndex.isValid()) {
        if (movingUp) {
            if (selectedVisualCoordinate < topVisualCoordinate) {
                // selecting something above the top: grow selection
                selectionModel()->select(newSelectedIndex, QItemSelectionModel::Rows | QItemSelectionModel::Select);
            } else {
                // selecting something below the top: shrink selection
                const QModelIndexList selectedIndexes = selection.indexes();
                for (const QModelIndex &idx : selectedIndexes) {
                    if ((idx.column() == 0) && (visualRect(idx).top() > selectedVisualCoordinate)) {
                        selectionModel()->select(idx, QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
                    }
                }
            }
        } else {
            if (selectedVisualCoordinate > bottomVisualCoordinate) {
                // selecting something below bottom: grow selection
                selectionModel()->select(newSelectedIndex, QItemSelectionModel::Rows | QItemSelectionModel::Select);
            } else {
                // selecting something above bottom: shrink selection
                const QModelIndexList selectedIndexes = selection.indexes();
                for (const QModelIndex &idx : selectedIndexes) {
                    if ((idx.column() == 0) && (visualRect(idx).top() < selectedVisualCoordinate)) {
                        selectionModel()->select(idx, QItemSelectionModel::Rows | QItemSelectionModel::Deselect);
                    }
                }
            }
        }
    } else {
        // no existing selection, just grow
        selectionModel()->select(newSelectedIndex, QItemSelectionModel::Rows | QItemSelectionModel::Select);
    }
}

bool View::selectNextMessageItem(MessageTypeFilter messageTypeFilter, ExistingSelectionBehaviour existingSelectionBehaviour, bool centerItem, bool loop)
{
    Item *it = nextMessageItem(messageTypeFilter, loop);
    if (!it) {
        return false;
    }

    if (it->parent() != d->mModel->rootItem()) {
        ensureDisplayedWithParentsExpanded(it);
    }

    QModelIndex idx = d->mModel->index(it, 0);

    Q_ASSERT(idx.isValid());

    switch (existingSelectionBehaviour) {
    case ExpandExistingSelection:
        selectionModel()->setCurrentIndex(idx, QItemSelectionModel::NoUpdate);
        selectionModel()->select(idx, QItemSelectionModel::Rows | QItemSelectionModel::Select);
        break;
    case GrowOrShrinkExistingSelection:
        selectionModel()->setCurrentIndex(idx, QItemSelectionModel::NoUpdate);
        growOrShrinkExistingSelection(idx, false);
        break;
    default:
        // case ClearExistingSelection:
        setCurrentIndex(idx);
        break;
    }

    if (centerItem) {
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }

    return true;
}

bool View::selectPreviousMessageItem(MessageTypeFilter messageTypeFilter, ExistingSelectionBehaviour existingSelectionBehaviour, bool centerItem, bool loop)
{
    Item *it = previousMessageItem(messageTypeFilter, loop);
    if (!it) {
        return false;
    }

    if (it->parent() != d->mModel->rootItem()) {
        ensureDisplayedWithParentsExpanded(it);
    }

    QModelIndex idx = d->mModel->index(it, 0);

    Q_ASSERT(idx.isValid());

    switch (existingSelectionBehaviour) {
    case ExpandExistingSelection:
        selectionModel()->setCurrentIndex(idx, QItemSelectionModel::NoUpdate);
        selectionModel()->select(idx, QItemSelectionModel::Rows | QItemSelectionModel::Select);
        break;
    case GrowOrShrinkExistingSelection:
        selectionModel()->setCurrentIndex(idx, QItemSelectionModel::NoUpdate);
        growOrShrinkExistingSelection(idx, true);
        break;
    default:
        // case ClearExistingSelection:
        setCurrentIndex(idx);
        break;
    }

    if (centerItem) {
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }

    return true;
}

bool View::focusNextMessageItem(MessageTypeFilter messageTypeFilter, bool centerItem, bool loop)
{
    Item *it = nextMessageItem(messageTypeFilter, loop);
    if (!it) {
        return false;
    }

    if (it->parent() != d->mModel->rootItem()) {
        ensureDisplayedWithParentsExpanded(it);
    }

    QModelIndex idx = d->mModel->index(it, 0);

    Q_ASSERT(idx.isValid());

    selectionModel()->setCurrentIndex(idx, QItemSelectionModel::NoUpdate);

    if (centerItem) {
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }

    return true;
}

bool View::focusPreviousMessageItem(MessageTypeFilter messageTypeFilter, bool centerItem, bool loop)
{
    Item *it = previousMessageItem(messageTypeFilter, loop);
    if (!it) {
        return false;
    }

    if (it->parent() != d->mModel->rootItem()) {
        ensureDisplayedWithParentsExpanded(it);
    }

    QModelIndex idx = d->mModel->index(it, 0);

    Q_ASSERT(idx.isValid());

    selectionModel()->setCurrentIndex(idx, QItemSelectionModel::NoUpdate);

    if (centerItem) {
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }

    return true;
}

void View::selectFocusedMessageItem(bool centerItem)
{
    QModelIndex idx = currentIndex();
    if (!idx.isValid()) {
        return;
    }

    if (selectionModel()->isSelected(idx)) {
        return;
    }

    selectionModel()->select(idx, QItemSelectionModel::Select | QItemSelectionModel::Current | QItemSelectionModel::Rows);

    if (centerItem) {
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }
}

bool View::selectFirstMessageItem(MessageTypeFilter messageTypeFilter, bool centerItem)
{
    if (!storageModel()) {
        return false; // nothing to do
    }

    Item *it = firstMessageItem(messageTypeFilter);
    if (!it) {
        return false;
    }

    Q_ASSERT(it != d->mModel->rootItem()); // must never happen (obviously)

    ensureDisplayedWithParentsExpanded(it);

    QModelIndex idx = d->mModel->index(it, 0);

    Q_ASSERT(idx.isValid());

    setCurrentIndex(idx);

    if (centerItem) {
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }

    return true;
}

bool View::selectLastMessageItem(MessageTypeFilter messageTypeFilter, bool centerItem)
{
    if (!storageModel()) {
        return false;
    }

    Item *it = lastMessageItem(messageTypeFilter);
    if (!it) {
        return false;
    }

    Q_ASSERT(it != d->mModel->rootItem());

    ensureDisplayedWithParentsExpanded(it);

    QModelIndex idx = d->mModel->index(it, 0);

    Q_ASSERT(idx.isValid());

    setCurrentIndex(idx);

    if (centerItem) {
        scrollTo(idx, QAbstractItemView::PositionAtCenter);
    }

    return true;
}

void View::modelFinishedLoading()
{
    Q_ASSERT(storageModel());
    Q_ASSERT(!d->mModel->isLoading());

    // nothing here for now :)
}

MessageItemSetReference View::createPersistentSet(const QList<MessageItem *> &items)
{
    return d->mModel->createPersistentSet(items);
}

QList<MessageItem *> View::persistentSetCurrentMessageItemList(MessageItemSetReference ref)
{
    return d->mModel->persistentSetCurrentMessageItemList(ref);
}

void View::deletePersistentSet(MessageItemSetReference ref)
{
    d->mModel->deletePersistentSet(ref);
}

void View::markMessageItemsAsAboutToBeRemoved(const QList<MessageItem *> &items, bool bMark)
{
    if (!bMark) {
        for (const auto mi : items) {
            if (mi->isValid()) { // hasn't been removed in the meantime
                mi->setAboutToBeRemoved(false);
            }
        }

        viewport()->update();

        return;
    }

    // ok.. we're going to mark the messages as "about to be deleted".
    // This means that we're going to make them non selectable.

    // What happens to the selection is generally an untrackable big mess.
    // Several components and entities are involved.

    // Qutie tries to apply some kind of internal logic in order to keep
    // "something" selected and "something" (else) to be current.
    // The results sometimes appear to depend on the current moon phase.

    // The Model will do crazy things in order to preserve the current
    // selection (and possibly the current item). If it's impossible then
    // it will make its own guesses about what should be selected next.
    // A problem is that the Model will do it one message at a time.
    // When item reparenting/reordering is involved then the guesses
    // can produce non-intuitive results.

    // Add the fact that selection and current item are distinct concepts,
    // their relative interaction depends on the settings and is often quite
    // unclear.

    // Add the fact that (at the time of writing) several styles don't show
    // the current item (only Yoda knows why) and this causes some confusion to the user.

    // Add the fact that the operations are asynchronous: deletion will start
    // a job, do some event loop processing and then complete the work at a later time.
    // The Qutie views also tend to accumulate the changes and perform them
    // all at once at the latest possible stage.

    // A radical approach is needed: we FIRST deal with the selection
    // by trying to move it away from the messages about to be deleted
    // and THEN mark the (hopefully no longer selected) messages as "about to be deleted".

    // First of all, find out if we're going to clear the entire selection (very likely).

    bool clearingEntireSelection = true;

    const QModelIndexList selectedIndexes = selectionModel()->selectedRows(0);

    if (selectedIndexes.count() > items.count()) {
        // the selection is bigger: we can't clear it completely
        clearingEntireSelection = false;
    } else {
        // the selection has same size or is smaller: we can clear it completely with our removal
        for (const QModelIndex &selectedIndex : selectedIndexes) {
            Q_ASSERT(selectedIndex.isValid());
            Q_ASSERT(selectedIndex.column() == 0);

            Item *selectedItem = static_cast<Item *>(selectedIndex.internalPointer());
            Q_ASSERT(selectedItem);

            if (selectedItem->type() != Item::Message) {
                continue;
            }

            if (!items.contains(static_cast<MessageItem *>(selectedItem))) {
                // the selection contains something that we aren't going to remove:
                // we will not clear the selection completely
                clearingEntireSelection = false;
                break;
            }
        }
    }

    if (clearingEntireSelection) {
        // Try to clear the current selection and select something sensible instead,
        // so after the deletion we will not end up with a random selection.
        // Pick up a message in the set (which is very likely to be contiguous), walk the tree
        // and select the next message that is NOT in the set.

        MessageItem *aMessage = items.last();
        Q_ASSERT(aMessage);

        // Avoid infinite loops by carrying only a limited number of attempts.
        // If there is any message that is not in the set then items.count() attempts should find it.
        int maxAttempts = items.count();

        while (items.contains(aMessage) && (maxAttempts > 0)) {
            Item *next = messageItemAfter(aMessage, MessageTypeAny, false);
            if (!next) {
                // no way
                aMessage = nullptr;
                break;
            }
            Q_ASSERT(next->type() == Item::Message);
            aMessage = static_cast<MessageItem *>(next);
            maxAttempts--;
        }

        if (!aMessage) {
            // try backwards
            aMessage = items.first();
            Q_ASSERT(aMessage);
            maxAttempts = items.count();

            while (items.contains(aMessage) && (maxAttempts > 0)) {
                Item *prev = messageItemBefore(aMessage, MessageTypeAny, false);
                if (!prev) {
                    // no way
                    aMessage = nullptr;
                    break;
                }
                Q_ASSERT(prev->type() == Item::Message);
                aMessage = static_cast<MessageItem *>(prev);
                maxAttempts--;
            }
        }

        if (aMessage) {
            QModelIndex aMessageIndex = d->mModel->index(aMessage, 0);
            Q_ASSERT(aMessageIndex.isValid());
            Q_ASSERT(static_cast<MessageItem *>(aMessageIndex.internalPointer()) == aMessage);
            Q_ASSERT(!selectionModel()->isSelected(aMessageIndex));
            setCurrentIndex(aMessageIndex);
            selectionModel()->select(aMessageIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        }
    } // else we aren't clearing the entire selection so something should just stay selected.

    // Now mark messages as about to be removed.

    for (const auto mi : items) {
        mi->setAboutToBeRemoved(true);
        QModelIndex idx = d->mModel->index(mi, 0);
        Q_ASSERT(idx.isValid());
        Q_ASSERT(static_cast<MessageItem *>(idx.internalPointer()) == mi);
        if (selectionModel()->isSelected(idx)) {
            selectionModel()->select(idx, QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
        }
    }

    viewport()->update();
}

void View::ensureDisplayedWithParentsExpanded(Item *it)
{
    Q_ASSERT(it);
    Q_ASSERT(it->parent());
    Q_ASSERT(it->isViewable()); // must be attached to the viewable root

    if (isRowHidden(it->parent()->indexOfChildItem(it), d->mModel->index(it->parent(), 0))) {
        setRowHidden(it->parent()->indexOfChildItem(it), d->mModel->index(it->parent(), 0), false);
    }

    it = it->parent();

    while (it->parent()) {
        if (isRowHidden(it->parent()->indexOfChildItem(it), d->mModel->index(it->parent(), 0))) {
            setRowHidden(it->parent()->indexOfChildItem(it), d->mModel->index(it->parent(), 0), false);
        }

        QModelIndex idx = d->mModel->index(it, 0);

        Q_ASSERT(idx.isValid());
        Q_ASSERT(static_cast<Item *>(idx.internalPointer()) == it);

        if (!isExpanded(idx)) {
            setExpanded(idx, true);
        }

        it = it->parent();
    }
}

bool View::isDisplayedWithParentsExpanded(Item *it) const
{
    // An item is currently viewable iff
    //  - it is marked as viewable in the item structure (that is, qt knows about its existence)
    //      (and this means that all of its parents are marked as viewable)
    //  - it is not explicitly hidden
    //  - all of its parents are expanded

    if (!it) {
        return false; // be nice and allow the caller not to care
    }

    if (!it->isViewable()) {
        return false; // item not viewable (not attached to the viewable root or qt not yet aware of it)
    }

    // the item and all the parents are marked as viewable.

    if (isRowHidden(it->parent()->indexOfChildItem(it), d->mModel->index(it->parent(), 0))) {
        return false; // item qt representation explicitly hidden
    }

    // the item (and theoretically all the parents) are not explicitly hidden

    // check the parent chain

    it = it->parent();

    while (it) {
        if (it == d->mModel->rootItem()) {
            return true; // parent is root item: ok
        }

        // parent is not root item

        if (!isExpanded(d->mModel->index(it, 0))) {
            return false; // parent is not expanded (so child not actually visible)
        }

        it = it->parent(); // climb up
    }

    // parent hierarchy interrupted somewhere
    return false;
}

bool View::isThreaded() const
{
    if (!d->mAggregation) {
        return false;
    }
    return d->mAggregation->threading() != Aggregation::NoThreading;
}

void View::slotSelectionChanged(const QItemSelection &, const QItemSelection &)
{
    // We assume that when selection changes, current item also changes.
    QModelIndex current = currentIndex();

    if (!current.isValid()) {
        d->mLastCurrentItem = nullptr;
        d->mWidget->viewMessageSelected(nullptr);
        d->mWidget->viewSelectionChanged();
        return;
    }

    if (!selectionModel()->isSelected(current)) {
        if (selectedIndexes().count() < 1) {
            // It may happen after row removals: Model calls this slot on currentIndex()
            // that actually might have changed "silently", without being selected.
            QItemSelection selection;
            selection.append(QItemSelectionRange(current));
            selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
            return; // the above recurses
        } else {
            // something is still selected anyway
            // This is probably a result of CTRL+Click which unselected current: leave it as it is.
            return;
        }
    }

    Item *it = static_cast<Item *>(current.internalPointer());
    Q_ASSERT(it);

    switch (it->type()) {
    case Item::Message:
        if (d->mLastCurrentItem != it) {
            qCDebug(MESSAGELIST_LOG) << "View message selected [" << static_cast<MessageItem *>(it)->subject() << "]";
            d->mWidget->viewMessageSelected(static_cast<MessageItem *>(it));
            d->mLastCurrentItem = it;
        }
        break;
    case Item::GroupHeader:
        if (d->mLastCurrentItem) {
            d->mWidget->viewMessageSelected(nullptr);
            d->mLastCurrentItem = nullptr;
        }
        break;
    default:
        // should never happen
        Q_ASSERT(false);
        break;
    }

    d->mWidget->viewSelectionChanged();
}

void View::mouseDoubleClickEvent(QMouseEvent *e)
{
    // Perform a hit test
    if (!d->mDelegate->hitTest(e->pos(), true)) {
        return;
    }

    // Something was hit :)

    Item *it = static_cast<Item *>(d->mDelegate->hitItem());
    if (!it) {
        return; // should never happen
    }

    switch (it->type()) {
    case Item::Message:
        // Let QTreeView handle the expansion
        QTreeView::mousePressEvent(e);

        switch (e->button()) {
        case Qt::LeftButton:

            if (d->mDelegate->hitContentItem()) {
                // Double clicking on clickable icons does NOT activate the message
                if (d->mDelegate->hitContentItem()->isIcon() && d->mDelegate->hitContentItem()->isClickable()) {
                    return;
                }
            }

            d->mWidget->viewMessageActivated(static_cast<MessageItem *>(it));
            break;
        default:
            // make gcc happy
            break;
        }
        break;
    case Item::GroupHeader:
        // Don't let QTreeView handle the selection (as it deselects the current messages)
        switch (e->button()) {
        case Qt::LeftButton:
            if (it->childItemCount() > 0) {
                // toggle expanded state
                setExpanded(d->mDelegate->hitIndex(), !isExpanded(d->mDelegate->hitIndex()));
            }
            break;
        default:
            // make gcc happy
            break;
        }
        break;
    default:
        // should never happen
        Q_ASSERT(false);
        break;
    }
}

void View::changeMessageStatusRead(MessageItem *it, bool read)
{
    Akonadi::MessageStatus set = it->status();
    Akonadi::MessageStatus unset = it->status();
    if (read) {
        set.setRead(true);
        unset.setRead(false);
    } else {
        set.setRead(false);
        unset.setRead(true);
    }
    viewport()->update();

    // This will actually request the widget to perform a status change on the storage.
    // The request will be then processed by the Model and the message will be updated again.

    d->mWidget->viewMessageStatusChangeRequest(it, set, unset);
}

void View::changeMessageStatus(MessageItem *it, Akonadi::MessageStatus set, Akonadi::MessageStatus unset)
{
    // We first change the status of MessageItem itself. This will make the change
    // visible to the user even if the Model is actually in the middle of a long job (maybe it's loading)
    // and can't process the status change request immediately.
    // Here we actually desynchronize the cache and trust that the later call to
    // d->mWidget->viewMessageStatusChangeRequest() will really perform the status change on the storage.
    // Well... in KMail it will unless something is really screwed. Anyway, if it will not, at the next
    // load the status will be just unchanged: no animals will be harmed.

    qint32 stat = it->status().toQInt32();
    stat |= set.toQInt32();
    stat &= ~(unset.toQInt32());
    Akonadi::MessageStatus status;
    status.fromQInt32(stat);
    it->setStatus(status);

    // Trigger an update so the immediate change will be shown to the user

    viewport()->update();

    // This will actually request the widget to perform a status change on the storage.
    // The request will be then processed by the Model and the message will be updated again.

    d->mWidget->viewMessageStatusChangeRequest(it, set, unset);
}

void View::mousePressEvent(QMouseEvent *e)
{
    d->mMousePressed = true;
    d->mLastMouseSource = e->source();

    if (d->mIsTouchEvent) {
        return;
    }

    d->onPressed(e);
}

void View::mouseMoveEvent(QMouseEvent *e)
{
    if (d->mIsTouchEvent && !d->mTapAndHoldActive) {
        return;
    }

    if (!(e->buttons() & Qt::LeftButton)) {
        QTreeView::mouseMoveEvent(e);
        return;
    }

    if (d->mMousePressPosition.isNull()) {
        return;
    }

    if ((e->pos() - d->mMousePressPosition).manhattanLength() <= QApplication::startDragDistance()) {
        return;
    }

    d->mTapAndHoldActive = false;
    if (d->mRubberBand->isVisible()) {
        d->mRubberBand->hide();
    }

    d->mWidget->viewStartDragRequest();
}

#if 0
void View::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
    QModelIndex index = currentIndex();
    if (index.isValid()) {
        QRect indexRect = this->visualRect(index);
        QPoint pos;

        if ((indexRect.isValid()) && (indexRect.bottom() > 0)) {
            if (indexRect.bottom() > viewport()->height()) {
                if (indexRect.top() <= viewport()->height()) {
                    pos = indexRect.topLeft();
                }
            } else {
                pos = indexRect.bottomLeft();
            }
        }

        Item *item = static_cast< Item * >(index.internalPointer());
        if (item) {
            if (item->type() == Item::GroupHeader) {
                d->mWidget->viewGroupHeaderContextPopupRequest(static_cast< GroupHeaderItem * >(item), viewport()->mapToGlobal(pos));
            } else if (!selectionEmpty()) {
                d->mWidget->viewMessageListContextPopupRequest(selectionAsMessageItemList(), viewport()->mapToGlobal(pos));
                e->accept();
            }
        }
    }
}

#endif

void View::dragEnterEvent(QDragEnterEvent *e)
{
    d->mWidget->viewDragEnterEvent(e);
}

void View::dragMoveEvent(QDragMoveEvent *e)
{
    d->mWidget->viewDragMoveEvent(e);
}

void View::dropEvent(QDropEvent *e)
{
    d->mWidget->viewDropEvent(e);
}

void View::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::FontChange:
        d->mDelegate->generalFontChanged();
        [[fallthrough]];
    case QEvent::PaletteChange:
    case QEvent::StyleChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::LocaleChange:
    case QEvent::LanguageChange:
        // All of these affect the theme's internal cache.
        setTheme(d->mTheme);
        // A layoutChanged() event will screw up the view state a bit.
        // Since this is a rare event we just reload the view.
        reload();
        break;
    default:
        // make gcc happy by default
        break;
    }

    QTreeView::changeEvent(e);
}

bool View::event(QEvent *e)
{
    if (e->type() == QEvent::TouchBegin) {
        d->mIsTouchEvent = true;
        d->mMousePressed = false;
        return false;
    }

    if (e->type() == QEvent::Gesture) {
        d->gestureEvent(static_cast<QGestureEvent *>(e));
        e->accept();
        return true;
    }

    // We catch ToolTip events and pass everything else

    if (e->type() != QEvent::ToolTip) {
        return QTreeView::event(e);
    }

    if (!MessageListSettings::self()->messageToolTipEnabled()) {
        return true; // don't display tooltips
    }

    auto he = dynamic_cast<QHelpEvent *>(e);
    if (!he) {
        return true; // eh ?
    }

    QPoint pnt = viewport()->mapFromGlobal(mapToGlobal(he->pos()));

    if (pnt.y() < 0) {
        return true; // don't display the tooltip for items hidden under the header
    }

    QModelIndex idx = indexAt(pnt);
    if (!idx.isValid()) {
        return true; // may be
    }

    Item *it = static_cast<Item *>(idx.internalPointer());
    if (!it) {
        return true; // hum
    }

    Q_ASSERT(storageModel());

    QColor bckColor = palette().color(QPalette::ToolTipBase);
    QColor txtColor = palette().color(QPalette::ToolTipText);
    QColor darkerColor(((bckColor.red() * 8) + (txtColor.red() * 2)) / 10,
                       ((bckColor.green() * 8) + (txtColor.green() * 2)) / 10,
                       ((bckColor.blue() * 8) + (txtColor.blue() * 2)) / 10);

    QString bckColorName = bckColor.name();
    QString txtColorName = txtColor.name();
    QString darkerColorName = darkerColor.name();
    const bool textIsLeftToRight = (QApplication::layoutDirection() == Qt::LeftToRight);
    const QString textDirection = textIsLeftToRight ? u"left"_s : QStringLiteral("right");

    QString tip = u"<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">"_s;

    switch (it->type()) {
    case Item::Message: {
        auto mi = static_cast<MessageItem *>(it);

        tip += QStringLiteral(
                   "<tr>"
                   "<td bgcolor=\"%1\" align=\"%4\" valign=\"middle\">"
                   "<div style=\"color: %2; font-weight: bold;\">"
                   "%3"
                   "</div>"
                   "</td>"
                   "</tr>")
                   .arg(txtColorName, bckColorName, mi->subject().toHtmlEscaped(), textDirection);

        tip += QLatin1StringView(
            "<tr>"
            "<td align=\"center\" valign=\"middle\">"
            "<table width=\"100%\" border=\"0\" cellpadding=\"2\" cellspacing=\"0\">");

        const QString htmlCodeForStandardRow = QStringLiteral(
            "<tr>"
            "<td align=\"right\" valign=\"top\" width=\"45\">"
            "<div style=\"font-weight: bold;\"><nobr>"
            "%1:"
            "</nobr></div>"
            "</td>"
            "<td align=\"left\" valign=\"top\">"
            "%2"
            "</td>"
            "</tr>");

        if (textIsLeftToRight) {
            tip += htmlCodeForStandardRow.arg(i18n("From"), mi->displaySender().toHtmlEscaped());
            tip += htmlCodeForStandardRow.arg(i18nc("Receiver of the email", "To"), mi->displayReceiver().toHtmlEscaped());
            tip += htmlCodeForStandardRow.arg(i18n("Date"), mi->formattedDate());
        } else {
            tip += htmlCodeForStandardRow.arg(mi->displaySender().toHtmlEscaped(), i18n("From"));
            tip += htmlCodeForStandardRow.arg(mi->displayReceiver().toHtmlEscaped(), i18nc("Receiver of the email", "To"));
            tip += htmlCodeForStandardRow.arg(mi->formattedDate(), i18n("Date"));
        }

        QString status = mi->statusDescription();
        const QString tags = mi->tagListDescription();
        if (!tags.isEmpty()) {
            if (!status.isEmpty()) {
                status += QLatin1StringView(", ");
            }
            status += tags;
        }

        if (textIsLeftToRight) {
            tip += htmlCodeForStandardRow.arg(i18n("Status"), status);
            tip += htmlCodeForStandardRow.arg(i18n("Size"), mi->formattedSize());
            tip += htmlCodeForStandardRow.arg(i18n("Folder"), mi->folder());
        } else {
            tip += htmlCodeForStandardRow.arg(status, i18n("Status"));
            tip += htmlCodeForStandardRow.arg(mi->formattedSize(), i18n("Size"));
            tip += htmlCodeForStandardRow.arg(mi->folder(), i18n("Folder"));
        }

        QString content = MessageList::Util::contentSummary(mi->akonadiItem());
        if (!content.trimmed().isEmpty()) {
            if (textIsLeftToRight) {
                tip += htmlCodeForStandardRow.arg(i18n("Preview"), content.replace(u'\n', u"<br>"_s));
            } else {
                tip += htmlCodeForStandardRow.arg(content.replace(u'\n', u"<br>"_s)).arg(i18n("Preview"));
            }
        }

        tip += QLatin1StringView(
            "</table>"
            "</td>"
            "</tr>");

        // FIXME: Find a way to show also CC and other header fields ?

        if (mi->hasChildren()) {
            Item::ChildItemStats stats;
            mi->childItemStats(stats);

            QString statsText;

            statsText = i18np("<b>%1</b> reply", "<b>%1</b> replies", mi->childItemCount());
            statsText += QLatin1StringView(", ");

            statsText += i18np("<b>%1</b> message in subtree (<b>%2</b> unread)",
                               "<b>%1</b> messages in subtree (<b>%2</b> unread)",
                               stats.mTotalChildCount,
                               stats.mUnreadChildCount);

            tip += QStringLiteral(
                       "<tr>"
                       "<td bgcolor=\"%1\" align=\"%3\" valign=\"middle\">"
                       "<nobr>%2</nobr>"
                       "</td>"
                       "</tr>")
                       .arg(darkerColorName, statsText, textDirection);
        }

        break;
    }
    case Item::GroupHeader: {
        auto ghi = static_cast<GroupHeaderItem *>(it);

        tip += QStringLiteral(
                   "<tr>"
                   "<td bgcolor=\"%1\" align=\"%4\" valign=\"middle\">"
                   "<div style=\"color: %2; font-weight: bold;\">"
                   "%3"
                   "</div>"
                   "</td>"
                   "</tr>")
                   .arg(txtColorName, bckColorName, ghi->label(), textDirection);

        QString description;

        switch (d->mAggregation->grouping()) {
        case Aggregation::GroupByDate:
            if (d->mAggregation->threading() != Aggregation::NoThreading) {
                switch (d->mAggregation->threadLeader()) {
                case Aggregation::TopmostMessage:
                    if (ghi->label().contains(QRegularExpression(u"[0-9]"_s))) {
                        description = i18nc("@info:tooltip Formats to something like 'Threads started on 2008-12-21'", "Threads started on %1", ghi->label());
                    } else {
                        description = i18nc("@info:tooltip Formats to something like 'Threads started Yesterday'", "Threads started %1", ghi->label());
                    }
                    break;
                case Aggregation::MostRecentMessage:
                    description = i18n("Threads with messages dated %1", ghi->label());
                    break;
                default:
                    // nuthin, make gcc happy
                    break;
                }
            } else {
                static const QRegularExpression reg(u"[0-9]"_s);
                if (ghi->label().contains(reg)) {
                    if (storageModel()->containsOutboundMessages()) {
                        description = i18nc("@info:tooltip Formats to something like 'Messages sent on 2008-12-21'", "Messages sent on %1", ghi->label());
                    } else {
                        description =
                            i18nc("@info:tooltip Formats to something like 'Messages received on 2008-12-21'", "Messages received on %1", ghi->label());
                    }
                } else {
                    if (storageModel()->containsOutboundMessages()) {
                        description = i18nc("@info:tooltip Formats to something like 'Messages sent Yesterday'", "Messages sent %1", ghi->label());
                    } else {
                        description = i18nc("@info:tooltip Formats to something like 'Messages received Yesterday'", "Messages received %1", ghi->label());
                    }
                }
            }
            break;
        case Aggregation::GroupByDateRange:
            if (d->mAggregation->threading() != Aggregation::NoThreading) {
                switch (d->mAggregation->threadLeader()) {
                case Aggregation::TopmostMessage:
                    description = i18n("Threads started within %1", ghi->label());
                    break;
                case Aggregation::MostRecentMessage:
                    description = i18n("Threads containing messages with dates within %1", ghi->label());
                    break;
                default:
                    // nuthin, make gcc happy
                    break;
                }
            } else {
                if (storageModel()->containsOutboundMessages()) {
                    description = i18n("Messages sent within %1", ghi->label());
                } else {
                    description = i18n("Messages received within %1", ghi->label());
                }
            }
            break;
        case Aggregation::GroupBySenderOrReceiver:
        case Aggregation::GroupBySender:
            if (d->mAggregation->threading() != Aggregation::NoThreading) {
                switch (d->mAggregation->threadLeader()) {
                case Aggregation::TopmostMessage:
                    description = i18n("Threads started by %1", ghi->label());
                    break;
                case Aggregation::MostRecentMessage:
                    description = i18n("Threads with most recent message by %1", ghi->label());
                    break;
                default:
                    // nuthin, make gcc happy
                    break;
                }
            } else {
                if (storageModel()->containsOutboundMessages()) {
                    if (d->mAggregation->grouping() == Aggregation::GroupBySenderOrReceiver) {
                        description = i18n("Messages sent to %1", ghi->label());
                    } else {
                        description = i18n("Messages sent by %1", ghi->label());
                    }
                } else {
                    description = i18n("Messages received from %1", ghi->label());
                }
            }
            break;
        case Aggregation::GroupByReceiver:
            if (d->mAggregation->threading() != Aggregation::NoThreading) {
                switch (d->mAggregation->threadLeader()) {
                case Aggregation::TopmostMessage:
                    description = i18n("Threads directed to %1", ghi->label());
                    break;
                case Aggregation::MostRecentMessage:
                    description = i18n("Threads with most recent message directed to %1", ghi->label());
                    break;
                default:
                    // nuthin, make gcc happy
                    break;
                }
            } else {
                if (storageModel()->containsOutboundMessages()) {
                    description = i18n("Messages sent to %1", ghi->label());
                } else {
                    description = i18n("Messages received by %1", ghi->label());
                }
            }
            break;
        default:
            // nuthin, make gcc happy
            break;
        }

        if (!description.isEmpty()) {
            tip += QStringLiteral(
                       "<tr>"
                       "<td align=\"%2\" valign=\"middle\">"
                       "%1"
                       "</td>"
                       "</tr>")
                       .arg(description, textDirection);
        }

        if (ghi->hasChildren()) {
            Item::ChildItemStats stats;
            ghi->childItemStats(stats);

            QString statsText;

            if (d->mAggregation->threading() != Aggregation::NoThreading) {
                statsText = i18np("<b>%1</b> thread", "<b>%1</b> threads", ghi->childItemCount());
                statsText += QLatin1StringView(", ");
            }

            statsText +=
                i18np("<b>%1</b> message (<b>%2</b> unread)", "<b>%1</b> messages (<b>%2</b> unread)", stats.mTotalChildCount, stats.mUnreadChildCount);

            tip += QStringLiteral(
                       "<tr>"
                       "<td bgcolor=\"%1\" align=\"%3\" valign=\"middle\">"
                       "<nobr>%2</nobr>"
                       "</td>"
                       "</tr>")
                       .arg(darkerColorName, statsText, textDirection);
        }

        break;
    }
    default:
        // nuthin (just make gcc happy for now)
        break;
    }

    tip += QLatin1StringView("</table>");

    QToolTip::showText(he->globalPos(), tip, viewport(), visualRect(idx));

    return true;
}

void View::slotExpandAllThreads()
{
    setAllThreadsExpanded(true);
}

void View::slotCollapseAllThreads()
{
    setAllThreadsExpanded(false);
}

void View::slotCollapseAllGroups()
{
    setAllGroupsExpanded(false);
}

void View::slotExpandAllGroups()
{
    setAllGroupsExpanded(true);
}

void View::slotCollapseCurrentItem()
{
    setCurrentThreadExpanded(false);
}

void View::slotExpandCurrentItem()
{
    setCurrentThreadExpanded(true);
}

void View::focusQuickSearch(const QString &selectedText)
{
    d->mWidget->focusQuickSearch(selectedText);
}

QList<Akonadi::MessageStatus> View::currentFilterStatus() const
{
    return d->mWidget->currentFilterStatus();
}

MessageList::Core::SearchMessageByButtons::SearchOptions View::currentOptions() const
{
    return d->mWidget->currentOptions();
}

QString View::currentFilterSearchString() const
{
    return d->mWidget->currentFilterSearchString();
}

QList<SearchLineCommand::SearchLineInfo> View::searchLineCommands() const
{
    return d->mWidget->searchLineCommands();
}

void View::setRowHidden(int row, const QModelIndex &parent, bool hide)
{
    const QModelIndex rowModelIndex = model()->index(row, 0, parent);
    const Item *const rowItem = static_cast<Item *>(rowModelIndex.internalPointer());

    if (rowItem) {
        const bool currentlyHidden = isRowHidden(row, parent);

        if (currentlyHidden != hide) {
            if (currentMessageItem() == rowItem) {
                selectionModel()->clear();
                selectionModel()->clearSelection();
            }
        }
    }

    QTreeView::setRowHidden(row, parent, hide);
}

void View::sortOrderMenuAboutToShow(QMenu *menu)
{
    d->mWidget->sortOrderMenuAboutToShow(menu);
}

void View::aggregationMenuAboutToShow(QMenu *menu)
{
    d->mWidget->aggregationMenuAboutToShow(menu);
}

void View::themeMenuAboutToShow(QMenu *menu)
{
    d->mWidget->themeMenuAboutToShow(menu);
}

void View::setCollapseItem(const QModelIndex &index)
{
    if (index.isValid()) {
        setExpanded(index, false);
    }
}

void View::setExpandItem(const QModelIndex &index)
{
    if (index.isValid()) {
        setExpanded(index, true);
    }
}

void View::setQuickSearchClickMessage(const QString &msg)
{
    d->mWidget->quickSearch()->setPlaceholderText(msg);
}

void View::ViewPrivate::onPressed(QMouseEvent *e)
{
    mMousePressPosition = QPoint();

    // Perform a hit test
    if (!mDelegate->hitTest(e->pos(), true)) {
        return;
    }

    // Something was hit :)

    Item *it = static_cast<Item *>(mDelegate->hitItem());
    if (!it) {
        return; // should never happen
    }

    // Abort any pending message pre-selection as the user is probably
    // already navigating the view (so pre-selection would make his view jump
    // to an unexpected place).
    mModel->setPreSelectionMode(PreSelectNone);

    switch (it->type()) {
    case Item::Message:
        mMousePressPosition = e->pos();

        switch (e->button()) {
        case Qt::LeftButton:
            // if we have multi selection then the meaning of hitting
            // the content item is quite unclear.
            if (mDelegate->hitContentItem() && (q->selectedIndexes().count() > 1)) {
                qCDebug(MESSAGELIST_LOG) << "Left hit with selectedIndexes().count() == " << q->selectedIndexes().count();

                switch (mDelegate->hitContentItem()->type()) {
                case Theme::ContentItem::ActionItemStateIcon:
                    q->changeMessageStatus(static_cast<MessageItem *>(it),
                                           it->status().isToAct() ? Akonadi::MessageStatus() : Akonadi::MessageStatus::statusToAct(),
                                           it->status().isToAct() ? Akonadi::MessageStatus::statusToAct() : Akonadi::MessageStatus());
                    return; // don't select the item
                    break;
                case Theme::ContentItem::ImportantStateIcon:
                    q->changeMessageStatus(static_cast<MessageItem *>(it),
                                           it->status().isImportant() ? Akonadi::MessageStatus() : Akonadi::MessageStatus::statusImportant(),
                                           it->status().isImportant() ? Akonadi::MessageStatus::statusImportant() : Akonadi::MessageStatus());
                    return; // don't select the item
                case Theme::ContentItem::ReadStateIcon:
                    q->changeMessageStatusRead(static_cast<MessageItem *>(it), it->status().isRead() ? false : true);
                    return;
                    break;
                case Theme::ContentItem::SpamHamStateIcon:
                    q->changeMessageStatus(static_cast<MessageItem *>(it),
                                           it->status().isSpam()
                                               ? Akonadi::MessageStatus()
                                               : (it->status().isHam() ? Akonadi::MessageStatus::statusSpam() : Akonadi::MessageStatus::statusHam()),
                                           it->status().isSpam() ? Akonadi::MessageStatus::statusSpam()
                                                                 : (it->status().isHam() ? Akonadi::MessageStatus::statusHam() : Akonadi::MessageStatus()));
                    return; // don't select the item
                    break;
                case Theme::ContentItem::WatchedIgnoredStateIcon:
                    q->changeMessageStatus(static_cast<MessageItem *>(it),
                                           it->status().isIgnored()
                                               ? Akonadi::MessageStatus()
                                               : (it->status().isWatched() ? Akonadi::MessageStatus::statusIgnored() : Akonadi::MessageStatus::statusWatched()),
                                           it->status().isIgnored()
                                               ? Akonadi::MessageStatus::statusIgnored()
                                               : (it->status().isWatched() ? Akonadi::MessageStatus::statusWatched() : Akonadi::MessageStatus()));
                    return; // don't select the item
                    break;
                default:
                    // make gcc happy
                    break;
                }
            }

            // Let QTreeView handle the selection and Q_EMIT the appropriate signals (slotSelectionChanged() may be called)
            q->QTreeView::mousePressEvent(e);

            break;
        case Qt::RightButton:
            // Let QTreeView handle the selection and Q_EMIT the appropriate signals (slotSelectionChanged() may be called)
            q->QTreeView::mousePressEvent(e);
            e->accept();
            mWidget->viewMessageListContextPopupRequest(q->selectionAsMessageItemList(), q->viewport()->mapToGlobal(e->pos()));

            break;
        default:
            // make gcc happy
            break;
        }
        break;
    case Item::GroupHeader: {
        // Don't let QTreeView handle the selection (as it deselects the current messages)
        auto groupHeaderItem = static_cast<GroupHeaderItem *>(it);

        switch (e->button()) {
        case Qt::LeftButton: {
            QModelIndex index = mModel->index(groupHeaderItem, 0);

            if (index.isValid()) {
                q->setCurrentIndex(index);
            }

            if (!mDelegate->hitContentItem()) {
                return;
            }

            if (mDelegate->hitContentItem()->type() == Theme::ContentItem::ExpandedStateIcon) {
                if (groupHeaderItem->childItemCount() > 0) {
                    // toggle expanded state
                    q->setExpanded(mDelegate->hitIndex(), !q->isExpanded(mDelegate->hitIndex()));
                }
            }
            break;
        }
        case Qt::RightButton:
            mWidget->viewGroupHeaderContextPopupRequest(groupHeaderItem, q->viewport()->mapToGlobal(e->pos()));
            break;
        default:
            // make gcc happy
            break;
        }
        break;
    }
    default:
        // should never happen
        Q_ASSERT(false);
        break;
    }
}

void View::ViewPrivate::gestureEvent(QGestureEvent *e)
{
    if (QGesture *gesture = e->gesture(Qt::TapGesture)) {
        tapTriggered(static_cast<QTapGesture *>(gesture));
    }
    if (QGesture *gesture = e->gesture(Qt::TapAndHoldGesture)) {
        tapAndHoldTriggered(static_cast<QTapAndHoldGesture *>(gesture));
    }
    if (QGesture *gesture = e->gesture(mTwoFingerTap)) {
        twoFingerTapTriggered(static_cast<KTwoFingerTap *>(gesture));
    }
}

void View::ViewPrivate::tapTriggered(QTapGesture *tap)
{
    static bool scrollerWasScrolling = false;

    if (tap->state() == Qt::GestureStarted) {
        mTapAndHoldActive = false;

        // if QScroller state is Scrolling or Dragging, the user makes the tap to stop the scrolling
        if (mScroller->state() == QScroller::Scrolling || mScroller->state() == QScroller::Dragging) {
            scrollerWasScrolling = true;
        } else if (mScroller->state() == QScroller::Pressed || mScroller->state() == QScroller::Inactive) {
            scrollerWasScrolling = false;
        }
    }

    if (tap->state() == Qt::GestureFinished && !scrollerWasScrolling) {
        mIsTouchEvent = false;

        // with touch you can touch multiple widgets at the same time, but only one widget will get a mousePressEvent.
        // we use this to select the right window
        if (!mMousePressed) {
            return;
        }

        if (mRubberBand->isVisible()) {
            mRubberBand->hide();
        }

        // simulate a mousePressEvent, to allow QTreeView to select the items
        QMouseEvent fakeMousePress(QEvent::MouseButtonPress,
                                   tap->position(),
                                   q->viewport()->mapToGlobal(tap->position()),
                                   mTapAndHoldActive ? Qt::RightButton : Qt::LeftButton,
                                   mTapAndHoldActive ? Qt::RightButton : Qt::LeftButton,
                                   Qt::NoModifier);

        onPressed(&fakeMousePress);
        mTapAndHoldActive = false;
    }

    if (tap->state() == Qt::GestureCanceled) {
        mIsTouchEvent = false;
        if (mRubberBand->isVisible()) {
            mRubberBand->hide();
        }
        mTapAndHoldActive = false;
    }
}

void View::ViewPrivate::tapAndHoldTriggered(QTapAndHoldGesture *tap)
{
    if (tap->state() == Qt::GestureFinished) {
        // with touch you can touch multiple widgets at the same time, but only one widget will get a mousePressEvent.
        // we use this to select the right window
        if (!mMousePressed) {
            return;
        }

        // the TapAndHoldGesture is triggerable the with mouse, we don't want this
        if (mLastMouseSource == Qt::MouseEventNotSynthesized) {
            return;
        }

        // the TapAndHoldGesture is triggerable the with stylus, we don't want this
        if (!mIsTouchEvent) {
            return;
        }

        mTapAndHoldActive = true;
        mScroller->stop();

        // simulate a mousePressEvent, to allow QTreeView to select the items
        const QPoint tapViewportPos(q->viewport()->mapFromGlobal(tap->position().toPoint()));
        QMouseEvent fakeMousePress(QEvent::MouseButtonPress, tapViewportPos, tapViewportPos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        onPressed(&fakeMousePress);

        const QPoint tapIndicatorSize(80, 80); // size for the tapAndHold indicator
        const QPoint pos(q->mapFromGlobal(tap->position().toPoint()));
        const QRect tapIndicatorRect(pos - (tapIndicatorSize / 2), pos + (tapIndicatorSize / 2));
        mRubberBand->setGeometry(tapIndicatorRect.normalized());
        mRubberBand->show();
    }
}

void View::ViewPrivate::twoFingerTapTriggered(KTwoFingerTap *tap)
{
    if (tap->state() == Qt::GestureFinished) {
        if (mTapAndHoldActive) {
            return;
        }

        // with touch you can touch multiple widgets at the same time, but only one widget will get a mousePressEvent.
        // we use this to select the right window
        if (!mMousePressed) {
            return;
        }

        // simulate a mousePressEvent with Qt::ControlModifier, to allow QTreeView to select the items
        QMouseEvent fakeMousePress(QEvent::MouseButtonPress,
                                   tap->pos(),
                                   q->viewport()->mapToGlobal(tap->pos()),
                                   Qt::LeftButton,
                                   Qt::LeftButton,
                                   Qt::ControlModifier);
        onPressed(&fakeMousePress);
    }
}

#include "moc_view.cpp"
