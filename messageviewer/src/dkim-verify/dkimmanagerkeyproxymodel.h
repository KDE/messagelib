/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <QSortFilterProxyModel>
namespace MessageViewer
{
class DKIMManagerKeyProxyModel : public QSortFilterProxyModel
{
public:
    explicit DKIMManagerKeyProxyModel(QObject *parent = nullptr);
    ~DKIMManagerKeyProxyModel() override;
};
}
