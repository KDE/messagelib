/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

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

    [[nodiscard]] QString filterText() const;
    void setFilterText(const QString &newFilterText);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString mFilterText;
};
}
