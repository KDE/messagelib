/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

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
    //    const auto leftData = source_left.data(MessageViewer::DKIMManagerKeyModel::StoredAtDateTimeRole).value<Akonadi::Collection>();
    //    const auto rightData = source_right.data(MessageViewer::DKIMManagerKeyModel::StoredAtDateTimeRole).value<Akonadi::Collection>();
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
