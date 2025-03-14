/*
   SPDX-FileCopyrightText: 2015-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class SendLaterDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit SendLaterDialogTest(QObject *parent = nullptr);
    ~SendLaterDialogTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void initTestCase();
};
