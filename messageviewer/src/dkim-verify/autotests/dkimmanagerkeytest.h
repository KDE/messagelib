/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMManagerKeyTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyTest(QObject *parent = nullptr);
    ~DKIMManagerKeyTest() override = default;
};
