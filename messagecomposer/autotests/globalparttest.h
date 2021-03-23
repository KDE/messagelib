/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include <QObject>

class GlobalPartTest : public QObject
{
    Q_OBJECT
public:
    explicit GlobalPartTest(QObject *parent = nullptr);
    ~GlobalPartTest();
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

