/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

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
