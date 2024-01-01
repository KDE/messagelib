/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

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
