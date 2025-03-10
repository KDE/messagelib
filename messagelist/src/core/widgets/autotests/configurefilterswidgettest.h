/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class ConfigureFiltersWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit ConfigureFiltersWidgetTest(QObject *parent = nullptr);
    ~ConfigureFiltersWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
