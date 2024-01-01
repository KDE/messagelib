/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMAuthenticationStatusInfoConverterTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMAuthenticationStatusInfoConverterTest(QObject *parent = nullptr);
    ~DKIMAuthenticationStatusInfoConverterTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
