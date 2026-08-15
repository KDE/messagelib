/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2026 Claudio Cambra <claudio.cambra@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *******************************************************************************/

#pragma once

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QRect>

#include "core/enums.h"

class QScrollBar;

namespace MessageList::Core
{
class Item;
class MessageItem;

class ModelViewInterface
{
public:
    virtual ~ModelViewInterface() = default;

    virtual QModelIndex currentIndex() const
    {
        return {};
    }

    virtual QItemSelectionModel *selectionModel() const
    {
        return nullptr;
    }

    virtual bool isExpanded(const QModelIndex &) const
    {
        return false;
    }

    virtual void expand(const QModelIndex &)
    {
    }

    virtual bool isAnimated() const
    {
        return false;
    }

    virtual void setAnimated(bool)
    {
    }

    virtual void setRootIsDecorated(bool)
    {
    }

    virtual void setRowHidden(int, const QModelIndex &, bool)
    {
    }

    virtual bool isRowHidden(int, const QModelIndex &) const
    {
        return false;
    }

    virtual void ensureDisplayedWithParentsExpanded(Item *)
    {
    }

    virtual Item *messageItemAfter(Item *, MessageTypeFilter, bool)
    {
        return nullptr;
    }

    virtual Item *messageItemBefore(Item *, MessageTypeFilter, bool)
    {
        return nullptr;
    }

    virtual int scrollingLockDirection() const
    {
        return 0;
    }

    virtual QRect visualRect(const QModelIndex &) const
    {
        return {};
    }

    virtual QScrollBar *verticalScrollBar() const
    {
        return nullptr;
    }

    virtual void ignoreUpdateGeometries(bool)
    {
    }

    virtual void updateGeometries()
    {
    }

    virtual void modelAboutToEmitLayoutChanged()
    {
    }

    virtual void modelEmittedLayoutChanged()
    {
    }

    virtual void ignoreCurrentChanges(bool)
    {
    }

    virtual void modelFinishedLoading()
    {
    }

    virtual bool selectFirstMessageItem(MessageTypeFilter, bool)
    {
        return false;
    }

    virtual void setCurrentMessageItem(MessageItem *, bool = false)
    {
    }

    virtual void setCurrentIndex(const QModelIndex &)
    {
    }

    virtual void slotSelectionChanged(const QItemSelection &, const QItemSelection &)
    {
    }
};
}
