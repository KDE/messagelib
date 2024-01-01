/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class DKIMUtilTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMUtilTest(QObject *parent = nullptr);
    ~DKIMUtilTest() override = default;
private Q_SLOTS:
    void shouldTestBodyCanonizationRelaxed();
    void shouldVerifyEmailDomain();
    void shouldVerifySubEmailDomain();
    void shouldConvertAuthenticationMethodEnumToString();
    void shouldConvertAuthenticationMethodToString();
};
