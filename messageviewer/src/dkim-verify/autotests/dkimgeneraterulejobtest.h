/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMGenerateRuleJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMGenerateRuleJobTest(QObject *parent = nullptr);
    ~DKIMGenerateRuleJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
