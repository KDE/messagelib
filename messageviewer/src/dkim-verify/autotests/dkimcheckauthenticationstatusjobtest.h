/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMCHECKAUTHENTICATIONSTATUSJOBTEST_H
#define DKIMCHECKAUTHENTICATIONSTATUSJOBTEST_H

#include <QObject>

class DKIMCheckAuthenticationStatusJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckAuthenticationStatusJobTest(QObject *parent = nullptr);
    ~DKIMCheckAuthenticationStatusJobTest() = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldTestMail();
    void shouldTestMail_data();
    void initTestCase();
};

#endif // DKIMCHECKAUTHENTICATIONSTATUSJOBTEST_H
