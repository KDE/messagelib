/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CONVERTSNIPPETVARIABLESUTILTEST_H
#define CONVERTSNIPPETVARIABLESUTILTEST_H

#include <QObject>

class ConvertSnippetVariablesUtilTest : public QObject
{
    Q_OBJECT
public:
    explicit ConvertSnippetVariablesUtilTest(QObject *parent = nullptr);
    ~ConvertSnippetVariablesUtilTest() = default;
private Q_SLOTS:
    void shouldTranslateVariables();
};

#endif // CONVERTSNIPPETVARIABLESUTILTEST_H
