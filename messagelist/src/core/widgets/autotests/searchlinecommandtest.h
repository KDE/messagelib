/*
  SPDX-FileCopyrightText: 2024-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class SearchLineCommandTest : public QObject
{
    Q_OBJECT
public:
    explicit SearchLineCommandTest(QObject *parent = nullptr);
    ~SearchLineCommandTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldParseInfo_data();
    void shouldParseInfo();
    void initTestCase();
    void shouldHaveSubType();
    void shouldBeValid();
    void mustBeUnique();
};
