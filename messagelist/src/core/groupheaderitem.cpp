/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/groupheaderitem.h"

using namespace MessageList::Core;

GroupHeaderItem::GroupHeaderItem(const QString &label)
    : Item(GroupHeader)
    , mLabel(label)
{
}

GroupHeaderItem::~GroupHeaderItem()
{
}

const QString &GroupHeaderItem::label() const
{
    return mLabel;
}

void GroupHeaderItem::setLabel(const QString &label)
{
    mLabel = label;
}
