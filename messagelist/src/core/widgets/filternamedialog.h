/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QDialog>

namespace MessageList
{
namespace Core
{
class FilterNameWidget;
class MESSAGELIST_TESTS_EXPORT FilterNameDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FilterNameDialog(QWidget *parent = nullptr);
    ~FilterNameDialog() override;

private:
    FilterNameWidget *const mFilterNameWidget;
};
}
}
