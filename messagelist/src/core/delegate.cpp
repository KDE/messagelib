/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/delegate.h"
#include "core/groupheaderitem.h"
#include "core/messageitem.h"
#include "core/view.h"

using namespace MessageList::Core;

Delegate::Delegate(View *pParent)
    : ThemeDelegate(pParent)
{
}

Delegate::~Delegate()
{
}

Item *Delegate::itemFromIndex(const QModelIndex &index) const
{
    return static_cast<Item *>(index.internalPointer());
}
