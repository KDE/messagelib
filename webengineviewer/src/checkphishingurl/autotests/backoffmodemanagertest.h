/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class BackOffModeManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit BackOffModeManagerTest(QObject *parent = nullptr);
    ~BackOffModeManagerTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldStartBackOffMode();

    void shouldIncreaseBackOff();
};
