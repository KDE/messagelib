/*
   SPDX-FileCopyrightText: 2019-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMUTILTEST_H
#define DKIMUTILTEST_H

#include <QObject>

class DKIMUtilTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMUtilTest(QObject *parent = nullptr);
    ~DKIMUtilTest() = default;
private Q_SLOTS:
    void shouldTestBodyCanonizationRelaxed();
    void shouldVerifyEmailDomain();
    void shouldVerifySubEmailDomain();
    void shouldConvertAuthenticationMethodEnumToString();
    void shouldConvertAuthenticationMethodToString();
};

#endif // DKIMUTILTEST_H
