/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMManagerRulesTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerRulesTest(QObject *parent = nullptr);
    ~DKIMManagerRulesTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldAddRules();
    void shouldClearRules();
};
