/*
   SPDX-FileCopyrightText: 2016-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class SearchFullHashJobTest : public QObject
{
    Q_OBJECT
public:
    explicit SearchFullHashJobTest(QObject *parent = nullptr);
    ~SearchFullHashJobTest() override;

private Q_SLOTS:
    void shouldNotBeAbleToStartWithEmptyUrl();
    void shouldCreateRequest_data();
    void shouldCreateRequest();
};
