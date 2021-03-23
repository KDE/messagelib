/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class VerifyDataBaseUpdateTest : public QObject
{
    Q_OBJECT
public:
    explicit VerifyDataBaseUpdateTest(QObject *parent = nullptr);
    ~VerifyDataBaseUpdateTest();
private Q_SLOTS:

    void shouldVerifyCheckSums();
    void shouldVerifyCheckSums_data();
};

