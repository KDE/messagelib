/*
   SPDX-FileCopyrightText: 2023-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeyproxymodel.h"
#include "dkimmanagerkeymodel.h"
using namespace MessageViewer;
DKIMManagerKeyProxyModel::DKIMManagerKeyProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
}

DKIMManagerKeyProxyModel::~DKIMManagerKeyProxyModel() = default;

bool DKIMManagerKeyProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    const int leftColumn{source_left.column()};
    if (leftColumn == MessageViewer::DKIMManagerKeyModel::StoredAtDateTimeRoleStr) {
        const QModelIndex leftMessageModelIndex = sourceModel()->index(source_left.row(), MessageViewer::DKIMManagerKeyModel::StoredAtDateTimeRole);
        const QModelIndex rightMessageModelIndex = sourceModel()->index(source_right.row(), MessageViewer::DKIMManagerKeyModel::StoredAtDateTimeRole);
        return QSortFilterProxyModel::lessThan(leftMessageModelIndex, rightMessageModelIndex);
    }
    if (leftColumn == MessageViewer::DKIMManagerKeyModel::LastUsedDateTimeRoleStr) {
        const QModelIndex leftMessageModelIndex = sourceModel()->index(source_left.row(), MessageViewer::DKIMManagerKeyModel::LastUsedDateTimeRole);
        const QModelIndex rightMessageModelIndex = sourceModel()->index(source_right.row(), MessageViewer::DKIMManagerKeyModel::LastUsedDateTimeRole);
        return QSortFilterProxyModel::lessThan(leftMessageModelIndex, rightMessageModelIndex);
    }
    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool DKIMManagerKeyProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (mFilterText.isEmpty()) {
        return true;
    }
    if (sourceModel()->index(source_row, MessageViewer::DKIMManagerKeyModel::DomainRole, source_parent).data().toString().contains(mFilterText)
        || sourceModel()->index(source_row, MessageViewer::DKIMManagerKeyModel::SelectorRole, source_parent).data().toString().contains(mFilterText)
        || sourceModel()->index(source_row, MessageViewer::DKIMManagerKeyModel::KeyRole, source_parent).data().toString().contains(mFilterText)) {
        return true;
    }
    return false;
}

QString DKIMManagerKeyProxyModel::filterText() const
{
    return mFilterText;
}

void DKIMManagerKeyProxyModel::setFilterText(const QString &newFilterText)
{
    if (mFilterText != newFilterText) {
        mFilterText = newFilterText;
        invalidateFilter();
    }
}
