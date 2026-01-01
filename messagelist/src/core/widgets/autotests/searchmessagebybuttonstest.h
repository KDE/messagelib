/*
  SPDX-FileCopyrightText: 2024-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QObject>

class SearchMessageByButtonsTest : public QObject
{
    Q_OBJECT
public:
    explicit SearchMessageByButtonsTest(QObject *parent = nullptr);
    ~SearchMessageByButtonsTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
