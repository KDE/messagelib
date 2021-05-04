/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QMenu>
#include <QObject>
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT FilterSavedMenu : public QMenu
{
    Q_OBJECT
public:
    explicit FilterSavedMenu(QWidget *parent = nullptr);
    ~FilterSavedMenu() override;

Q_SIGNALS:
    void saveFilter();
    void configureFilters();

private:
    void slotShowMenu();
};
}
}
