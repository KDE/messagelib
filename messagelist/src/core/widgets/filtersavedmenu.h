/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QMenu>
#include <QObject>
namespace MessageList
{
namespace Core
{
class FilterSavedMenu : public QMenu
{
    Q_OBJECT
public:
    explicit FilterSavedMenu(QWidget *parent = nullptr);
    ~FilterSavedMenu() override;

private:
    void slotShowMenu();
};
}
}
