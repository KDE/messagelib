/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DeveloperToolWidgetTest : public QObject
{
    Q_OBJECT
public:
    explicit DeveloperToolWidgetTest(QObject *parent = nullptr);
    ~DeveloperToolWidgetTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
