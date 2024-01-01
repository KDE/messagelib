/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMManagerKeyDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyDialogTest(QObject *parent = nullptr);
    ~DKIMManagerKeyDialogTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};
