/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMCheckAuthenticationStatusJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckAuthenticationStatusJobTest(QObject *parent = nullptr);
    ~DKIMCheckAuthenticationStatusJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldTestMail();
    void shouldTestMail_data();
    void initTestCase();
};
