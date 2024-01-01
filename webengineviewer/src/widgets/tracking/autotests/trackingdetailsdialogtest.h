/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class TrackingDetailsDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit TrackingDetailsDialogTest(QObject *parent = nullptr);
    ~TrackingDetailsDialogTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};
