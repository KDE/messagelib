/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef VERIFYDATABASEUPDATETEST_H
#define VERIFYDATABASEUPDATETEST_H

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

#endif // VERIFYDATABASEUPDATETEST_H
