/*
   SPDX-FileCopyrightText: 2020-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "searchlinecommandflowlayout.h"

#include <QStyle>
#include <QWidget>
using namespace Qt::Literals::StringLiterals;
using namespace MessageList::Core;
static int smartSpacing(QObject *parent, QStyle::PixelMetric pm)
{
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        auto pw = static_cast<QWidget *>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return static_cast<QLayout *>(parent)->spacing();
    }
}

SearchLineCommandFlowLayout::SearchLineCommandFlowLayout(QWidget *parent)
    : QLayout(parent)
{
}

SearchLineCommandFlowLayout::~SearchLineCommandFlowLayout()
{
    clear();
}

int SearchLineCommandFlowLayout::horizontalSpacing() const
{
    if (mHorizontalSpacing >= 0) {
        return mHorizontalSpacing;
    } else {
        return smartSpacing(parent(), QStyle::PM_LayoutHorizontalSpacing);
    }
}

void SearchLineCommandFlowLayout::setHorizontalSpacing(int horizontalSpacing)
{
    if (mHorizontalSpacing != horizontalSpacing) {
        mHorizontalSpacing = horizontalSpacing;
        invalidate();
    }
}

int SearchLineCommandFlowLayout::verticalSpacing() const
{
    if (mVerticalSpacing >= 0) {
        return mVerticalSpacing;
    } else {
        return smartSpacing(parent(), QStyle::PM_LayoutVerticalSpacing);
    }
}

void SearchLineCommandFlowLayout::setVerticalSpacing(int verticalSpacing)
{
    if (mVerticalSpacing != verticalSpacing) {
        mVerticalSpacing = verticalSpacing;
        invalidate();
    }
}

QSize SearchLineCommandFlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize SearchLineCommandFlowLayout::minimumSize() const
{
    const QMargins margins = contentsMargins();
    QSize size;

    for (const QLayoutItem *item : mItems) {
        size = size.expandedTo(item->minimumSize());
    }

    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

void SearchLineCommandFlowLayout::addItem(QLayoutItem *item)
{
    Q_ASSERT(!mItems.contains(item));
    mItems.append(item);
    invalidate();
}

QLayoutItem *SearchLineCommandFlowLayout::itemAt(int index) const
{
    if (index >= 0 && index < mItems.count()) {
        return mItems[index];
    }

    return nullptr;
}

QLayoutItem *SearchLineCommandFlowLayout::takeAt(int index)
{
    if (index >= 0 && index < mItems.count()) {
        auto *it = mItems.takeAt(index);
        invalidate();
        return it;
    }

    return nullptr;
}

int SearchLineCommandFlowLayout::count() const
{
    return mItems.count();
}

Qt::Orientations SearchLineCommandFlowLayout::expandingDirections() const
{
    return {};
}

bool SearchLineCommandFlowLayout::hasHeightForWidth() const
{
    return true;
}

int SearchLineCommandFlowLayout::heightForWidth(int width) const
{
    return doFlow(QRect(0, 0, width, 0), false);
}

void SearchLineCommandFlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doFlow(rect, true);
}

void SearchLineCommandFlowLayout::clear()
{
    while (QLayoutItem *item = takeAt(0)) {
        delete item;
    }
}

void SearchLineCommandFlowLayout::clearAndDeleteWidgets()
{
    while (QLayoutItem *item = takeAt(0)) {
        item->widget()->deleteLater();
        delete item;
    }
}

int SearchLineCommandFlowLayout::doFlow(QRect rect, bool effective) const
{
    const QMargins margins = contentsMargins();
    const QRect effectiveRect = rect.adjusted(margins.left(), margins.top(), -margins.right(), -margins.bottom());
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int highest = 0;

    for (QLayoutItem *item : mItems) {
        const QWidget *widget = item->widget();

        if (!widget->isVisibleTo(parentWidget())) {
            continue;
        }

        int hSpacing = horizontalSpacing();
        int vSpacing = verticalSpacing();

        if (hSpacing == -1) {
            hSpacing = widget->style()->layoutSpacing(QSizePolicy::Frame, QSizePolicy::Frame, Qt::Horizontal);
        }

        if (vSpacing == -1) {
            vSpacing = widget->style()->layoutSpacing(QSizePolicy::Frame, QSizePolicy::Frame, Qt::Vertical);
        }

        int widgetXPos = x + item->sizeHint().width() + hSpacing;

        if (widgetXPos - hSpacing > effectiveRect.right() && highest > 0) {
            x = effectiveRect.x();
            y += highest + vSpacing;
            widgetXPos = x + item->sizeHint().width() + hSpacing;
            highest = 0;
        }

        if (effective) {
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        }

        x = widgetXPos;
        highest = qMax(highest, item->sizeHint().height());
    }

    return y + highest - rect.y() + margins.bottom();
}

#include "moc_searchlinecommandflowlayout.cpp"
