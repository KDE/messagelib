/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerTest(QObject *parent = nullptr);
    ~DKIMManagerTest() override = default;
};

