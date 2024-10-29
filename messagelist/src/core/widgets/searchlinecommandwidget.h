/*
  SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "messagelist_private_export.h"
#include <QWidget>
class QLabel;
namespace MessageList
{
namespace Core
{
class SearchLineCommandButtonsWidget;
class MESSAGELIST_TESTS_EXPORT SearchLineCommandWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchLineCommandWidget(QWidget *parent = nullptr);
    ~SearchLineCommandWidget() override;

    void setLabel(const QString &str);
Q_SIGNALS:
    void insertCommand(const QString &commandStr);

private:
    SearchLineCommandButtonsWidget *const mSearchLineCommandButtonsWidget;
    QLabel *const mLabel;
};
}
}
