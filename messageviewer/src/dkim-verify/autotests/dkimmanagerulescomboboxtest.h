/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMManageRulesComboBoxTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManageRulesComboBoxTest(QObject *parent = nullptr);
    ~DKIMManageRulesComboBoxTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldChangeIndex();
};

