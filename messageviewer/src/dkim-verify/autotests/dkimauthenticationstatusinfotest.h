/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMAuthenticationStatusInfoTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMAuthenticationStatusInfoTest(QObject *parent = nullptr);
    ~DKIMAuthenticationStatusInfoTest() override = default;

private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldParseKey();
    void shouldParseKey_data();
};
