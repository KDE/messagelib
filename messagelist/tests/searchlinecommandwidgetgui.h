/*
   SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QWidget>
class QLineEdit;
class SearchLineCommandWidgetGui : public QWidget
{
    Q_OBJECT
public:
    explicit SearchLineCommandWidgetGui(QWidget *parent = nullptr);
    ~SearchLineCommandWidgetGui() override;

private:
    QLineEdit *const mLineEdit;
};
