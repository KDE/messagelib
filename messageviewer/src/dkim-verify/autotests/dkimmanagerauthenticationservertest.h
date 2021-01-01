/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMMANAGERAUTHENTICATIONSERVERTEST_H
#define DKIMMANAGERAUTHENTICATIONSERVERTEST_H

#include <QObject>

class DKIMManagerAuthenticationServerTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMManagerAuthenticationServerTest(QObject *parent = nullptr);
    ~DKIMManagerAuthenticationServerTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValue();
};

#endif // DKIMMANAGERAUTHENTICATIONSERVERTEST_H
