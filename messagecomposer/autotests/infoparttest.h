/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class InfoPartTest : public QObject
{
    Q_OBJECT
public:
    explicit InfoPartTest(QObject *parent = nullptr);
    ~InfoPartTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};
