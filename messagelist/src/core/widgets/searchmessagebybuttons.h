/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "messagelist_private_export.h"
#include <QWidget>
class QHBoxLayout;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchMessageByButtons : public QWidget
{
    Q_OBJECT
public:
    explicit SearchMessageByButtons(QWidget *parent = nullptr);
    ~SearchMessageByButtons() override;

private:
    MESSAGELIST_NO_EXPORT void createButtons();
    QHBoxLayout *const mMainLayout;
};
}
}
