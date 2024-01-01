/*
   SPDX-FileCopyrightText: 2022-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class OpenWithUrlInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit OpenWithUrlInfoTest(QObject *parent = nullptr);
    ~OpenWithUrlInfoTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
