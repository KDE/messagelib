/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class SearchLineStatusTest : public QObject
{
    Q_OBJECT
public:
    explicit SearchLineStatusTest(QObject *parent = nullptr);
    ~SearchLineStatusTest() override;

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldAddCompletionItem();
    void shouldClearCompleter();
};
