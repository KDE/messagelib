/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONVERTVARIABLESJOBTEST_H
#define CONVERTVARIABLESJOBTEST_H

#include <QObject>

class ConvertVariablesJobTest : public QObject
{
    Q_OBJECT
public:
    explicit ConvertVariablesJobTest(QObject *parent = nullptr);
    ~ConvertVariablesJobTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldCanStart();
    void shouldConvertVariables();
    void shouldConvertVariables_data();
};

#endif // CONVERTVARIABLESJOBTEST_H
