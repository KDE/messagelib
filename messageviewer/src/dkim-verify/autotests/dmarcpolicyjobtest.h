/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DMARCPOLICYJOBTEST_H
#define DMARCPOLICYJOBTEST_H

#include <QObject>

class DMARCPolicyJobTest : public QObject
{
    Q_OBJECT
public:
    explicit DMARCPolicyJobTest(QObject *parent = nullptr);
    ~DMARCPolicyJobTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};

#endif // DMARCPOLICYJOBTEST_H
