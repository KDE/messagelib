/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QObject>

class SearchLineCommandWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit SearchLineCommandWidgetTest(QObject *parent = nullptr);
    ~SearchLineCommandWidgetTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
};
