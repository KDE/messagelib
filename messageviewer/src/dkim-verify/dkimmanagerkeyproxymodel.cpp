/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeyproxymodel.h"
using namespace MessageViewer;
DKIMManagerKeyProxyModel::DKIMManagerKeyProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
}

DKIMManagerKeyProxyModel::~DKIMManagerKeyProxyModel() = default;
