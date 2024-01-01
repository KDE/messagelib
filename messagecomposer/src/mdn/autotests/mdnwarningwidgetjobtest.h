/*
  SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#pragma once

#include <QObject>

class MDNWarningWidgetJobTest : public QObject
{
    Q_OBJECT
public:
    explicit MDNWarningWidgetJobTest(QObject *parent = nullptr);
    ~MDNWarningWidgetJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
