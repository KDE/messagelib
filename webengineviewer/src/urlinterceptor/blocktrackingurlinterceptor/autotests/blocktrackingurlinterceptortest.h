/*
   SPDX-FileCopyrightText: 2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class BlockTrackingUrlInterceptorTest : public QObject
{
    Q_OBJECT
public:
    explicit BlockTrackingUrlInterceptorTest(QObject *parent = nullptr);
    ~BlockTrackingUrlInterceptorTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldNotInterceptWhenDisabled();
    void shouldInterceptTracker_data();
    void shouldInterceptTracker();
    void shouldNotInterceptRegularUrl_data();
    void shouldNotInterceptRegularUrl();
};
