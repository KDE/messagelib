/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeytreeview.h"
using namespace MessageViewer;
DKIMManagerKeyTreeView::DKIMManagerKeyTreeView(QWidget *parent)
    : QTreeView(parent)
{
    setRootIsDecorated(false);
    setAlternatingRowColors(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(ExtendedSelection);
    setUniformRowHeights(true);
}

DKIMManagerKeyTreeView::~DKIMManagerKeyTreeView() = default;

void DKIMManagerKeyTreeView::setFilterStr(const QString &str)
{
    // TODO
}
