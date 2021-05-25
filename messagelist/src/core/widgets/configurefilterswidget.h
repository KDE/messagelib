/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_private_export.h"
#include <QWidget>
class QListWidget;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_TESTS_EXPORT ConfigureFiltersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigureFiltersWidget(QWidget *parent = nullptr);
    ~ConfigureFiltersWidget() override;

private:
    void init();
    QListWidget *const mListFiltersWidget;
};
}
}
