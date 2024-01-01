/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class CheckPhishingUrlCacheTest : public QObject
{
    Q_OBJECT
public:
    explicit CheckPhishingUrlCacheTest(QObject *parent = nullptr);
    ~CheckPhishingUrlCacheTest() override;
private Q_SLOTS:
    void shouldNotBeAMalware();

    void shouldAddValue_data();
    void shouldAddValue();

    void shouldStoreValues();
};
