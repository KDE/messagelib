/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagelist_private_export.h"
#include <Akonadi/MessageStatus>
#include <QWidget>
class QButtonGroup;
class QHBoxLayout;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchStatusButtons : public QWidget
{
    Q_OBJECT
public:
    explicit SearchStatusButtons(QWidget *parent = nullptr);
    ~SearchStatusButtons() override;

    void clearFilter();
    void setFilterMessageStatus(const QList<Akonadi::MessageStatus> &newLstStatus);
Q_SIGNALS:
    void filterStatusChanged(const QList<Akonadi::MessageStatus> &lst);

private:
    MESSAGELIST_NO_EXPORT void createAction();
    MESSAGELIST_NO_EXPORT void createFilterAction(const QIcon &icon, const QString &text, int value);
    MESSAGELIST_NO_EXPORT void updateFilters();
    QButtonGroup *const mButtonGroup;
    QHBoxLayout *const mMainLayout;
};
}
}
