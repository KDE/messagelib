/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class FilterNameWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit FilterNameWidgetTest(QObject *parent = nullptr);
    ~FilterNameWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
