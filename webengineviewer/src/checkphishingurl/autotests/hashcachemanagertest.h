/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class HashCacheManagerTest : public QObject
{
    Q_OBJECT
public:
    explicit HashCacheManagerTest(QObject *parent = nullptr);
    ~HashCacheManagerTest();
private Q_SLOTS:
    void shouldBeUnknowByDefault();

    void shouldAddValue_data();
    void shouldAddValue();
};

