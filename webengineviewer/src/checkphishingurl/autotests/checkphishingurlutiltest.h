/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class CheckPhishingUrlUtilTest : public QObject
{
    Q_OBJECT
public:
    explicit CheckPhishingUrlUtilTest(QObject *parent = nullptr);
    ~CheckPhishingUrlUtilTest() override;
private Q_SLOTS:
    void shouldConvertToSecond_data();
    void shouldConvertToSecond();

    void shouldCacheIsStillValid_data();
    void shouldCacheIsStillValid();

    void shouldGenerateBackModeDelay_data();
    void shouldGenerateBackModeDelay();
};
