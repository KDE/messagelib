/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMWidgetInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMWidgetInfoTest(QObject *parent = nullptr);
    ~DKIMWidgetInfoTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldClearWidget();
};

