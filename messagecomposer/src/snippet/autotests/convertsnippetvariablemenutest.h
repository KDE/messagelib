/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONVERTSNIPPETVARIABLEMENUTEST_H
#define CONVERTSNIPPETVARIABLEMENUTEST_H

#include <QObject>

class ConvertSnippetVariableMenuTest : public QObject
{
    Q_OBJECT
public:
    explicit ConvertSnippetVariableMenuTest(QObject *parent = nullptr);
    ~ConvertSnippetVariableMenuTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // CONVERTSNIPPETVARIABLEMENUTEST_H
