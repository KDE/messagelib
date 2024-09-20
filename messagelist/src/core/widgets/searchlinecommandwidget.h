/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QWidget>
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT SearchLineCommandWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchLineCommandWidget(QWidget *parent = nullptr);
    ~SearchLineCommandWidget() override;
};
}
}
