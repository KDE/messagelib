/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include <QObject>

class TextPartTest : public QObject
{
    Q_OBJECT
public:
    explicit TextPartTest(QObject *parent = nullptr);
    ~TextPartTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};
