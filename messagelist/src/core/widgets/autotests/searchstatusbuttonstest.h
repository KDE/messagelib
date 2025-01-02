/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class SearchStatusButtonsTest : public QObject
{
    Q_OBJECT
public:
    explicit SearchStatusButtonsTest(QObject *parent = nullptr);
    ~SearchStatusButtonsTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
